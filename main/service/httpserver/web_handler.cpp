//
// Created by eugen on 7/11/2026.
//

#include "web_handler.hpp"
#include <cJSON.h>
#include <cstring>
#include <esp_http_server.h>
#include <esp_littlefs.h>
#include <esp_log.h>
#include <esp_vfs.h>
#include <etl/string.h>
#include <fcntl.h>

#define CHECK_FILE_EXTENSION(filename, ext)                                    \
	(strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

using namespace svc::httpserver;

const char *TAG = "esp-rest";

// Stores references to the shared LCD/NVS/Wi-Fi services so route
// handlers (bound via httpd_uri_t.user_ctx) can reach them.
WebHandler::WebHandler(ILcdService &lcd_svc, INvsService &nvs_svc,
					   IWifiService &wifi_svc) :
	lcd_svc_(lcd_svc), nvs_svc_(nvs_svc), wifi_svc_(wifi_svc) {}

esp_err_t WebHandler::login(httpd_req_t *req) {
	char buffer[1024] = {};
	esp_err_t err = parse_buffer(req, buffer, sizeof(buffer));
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "login - Failed to parse buffer");
		httpd_resp_send_500(req);
		return err;
	}

	// Parse JSON
	cJSON *root = cJSON_Parse(buffer);
	if (root == nullptr) {
		httpd_resp_send_500(req);
		return ESP_FAIL;
	}

	cJSON *username_item = cJSON_GetObjectItem(root, "ssid");
	cJSON *password_item = cJSON_GetObjectItem(root, "password");

	etl::string_view username;
	etl::string_view password;

	if (cJSON_IsString(username_item) &&
		(username_item->valuestring != nullptr)) {
		username = username_item->valuestring;
	} else {
		cJSON_Delete(root);
		httpd_resp_send_500(req);
		return ESP_FAIL;
	}

	if (cJSON_IsString(password_item) &&
		(password_item->valuestring != nullptr)) {
		password = password_item->valuestring;
	} else {
		httpd_resp_send_500(req);
		cJSON_Delete(root);
		return ESP_FAIL;
	}

	ESP_LOGI(TAG, "Received ssid: %s, password: %s", username.data(),
			 password.data());

	nvs_svc_.write_blob("wifi", "ssid", username.data(),
						username.length() + 1); // include null terminator
	nvs_svc_.write_blob("wifi", "password", password.data(),
						password.length() + 1); // include null terminator

	cJSON_Delete(root);

	const etl::string<128> payload = R"({"status": "OK"})";
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, payload.c_str(), payload.length());
	return ESP_OK;
}

esp_err_t WebHandler::parse_buffer(httpd_req_t *req, char *buffer,
								   size_t buf_size) {
	int total_len = req->content_len;
	int cur_len = 0;
	int received = 0;

	if (total_len >= static_cast<int>(buf_size)) {
		return ESP_FAIL;
	}

	// Read the body (may arrive in chunks)
	while (cur_len < total_len) {
		received = httpd_req_recv(req, buffer + cur_len, total_len - cur_len);
		if (received <= 0) {
			return ESP_FAIL;
		}
		cur_len += received;
	}
	buffer[total_len] = '\0';
	return ESP_OK;
}

// GET /health handler: replies with a static JSON OK payload.
esp_err_t WebHandler::healthcheck(httpd_req_t *req) {
	const etl::string<128> payload = R"({"status": "OK"})";
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, payload.c_str(), payload.length());
	return ESP_OK;
}

// Serve static files from filesystem
// Copied from (https://tinyurl.com/mwp4tjtd)
esp_err_t WebHandler::serve_static_files(httpd_req_t *req) {
	char filepath[FILE_PATH_MAX];

	auto *rest_context = static_cast<rest_server_context_t *>(req->user_ctx);
	strlcpy(filepath, rest_context->base_path, sizeof(filepath));
	if (req->uri[strlen(req->uri) - 1] == '/') {
		strlcat(filepath, "/index.html", sizeof(filepath));
	} else {
		strlcat(filepath, req->uri, sizeof(filepath));
	}
	int fd = open(filepath, O_RDONLY, 0);
	if (fd == -1) {
		// Asset not found: fall back to index.html so the SPA router can handle
		// client-side routes (e.g. /settings) that have no matching file on disk.
		strlcpy(filepath, rest_context->base_path, sizeof(filepath));
		strlcat(filepath, "/index.html", sizeof(filepath));
		fd = open(filepath, O_RDONLY, 0);
	}
	if (fd == -1) {
		// index.html itself is missing — LittleFS partition was likely not flashed.
		// Run 'idf.py flash' (not 'app-flash') to include the www partition image.
		ESP_LOGE(TAG, "Failed to open file : %s", filepath);
		httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
		return ESP_FAIL;
	}

	set_content_type_from_file(req, filepath);

	char *chunk = rest_context->scratch;
	ssize_t read_bytes;
	do {
		/* Read file in chunks into the scratch buffer */
		read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
		if (read_bytes == -1) {
			ESP_LOGE(TAG, "Failed to read file : %s", filepath);
		} else if (read_bytes > 0) {
			/* Send the buffer contents as HTTP response chunk */
			if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
				close(fd);
				ESP_LOGE(TAG, "File sending failed!");
				/* Abort sending file */
				httpd_resp_sendstr_chunk(req, NULL);
				/* Respond with 500 Internal Server Error */
				httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
									"Failed to send file");
				return ESP_FAIL;
			}
		}
	} while (read_bytes > 0);
	/* Close file after sending complete */
	close(fd);
	/* Respond with an empty chunk to signal HTTP response completion */
	httpd_resp_send_chunk(req, NULL, 0);
	ESP_LOGI(TAG, "File sending complete");
	return ESP_OK;
}

/* Set HTTP response content type according to file extension */
esp_err_t WebHandler::set_content_type_from_file(httpd_req_t *req,
												 const char *filepath) {
	const char *type = "text/plain";
	if (CHECK_FILE_EXTENSION(filepath, ".html")) {
		type = "text/html";
	} else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
		type = "application/javascript";
	} else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
		type = "text/css";
	} else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
		type = "image/png";
	} else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
		type = "image/x-icon";
	} else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
		type = "text/xml";
	}
	return httpd_resp_set_type(req, type);
}

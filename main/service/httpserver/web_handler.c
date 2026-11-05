//
// Native C counterpart of web_handler.cpp.
//

#include "web_handler.h"

#include <cJSON.h>
#include <esp_log.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

static const char TAG[] = "web_handler";

static const char OK_PAYLOAD[] = "{\"status\": \"OK\"}";

static bool has_file_extension(const char *filename, const char *ext) {
	size_t name_len = strlen(filename);
	size_t ext_len = strlen(ext);
	return name_len >= ext_len &&
		   strcasecmp(&filename[name_len - ext_len], ext) == 0;
}

// Sets the HTTP response content type according to the file extension.
static esp_err_t set_content_type_from_file(httpd_req_t *req,
											const char *filepath) {
	const char *type = "text/plain";
	if (has_file_extension(filepath, ".html")) {
		type = "text/html";
	} else if (has_file_extension(filepath, ".js")) {
		type = "application/javascript";
	} else if (has_file_extension(filepath, ".css")) {
		type = "text/css";
	} else if (has_file_extension(filepath, ".png")) {
		type = "image/png";
	} else if (has_file_extension(filepath, ".ico")) {
		type = "image/x-icon";
	} else if (has_file_extension(filepath, ".svg")) {
		type = "text/xml";
	}
	return httpd_resp_set_type(req, type);
}

// Reads the full request body (which may arrive in chunks) into buffer;
// fails if the body would not fit in buf_size.
static esp_err_t parse_buffer(httpd_req_t *req, char *buffer,
							  size_t buf_size) {
	size_t total_len = req->content_len;
	size_t cur_len = 0;

	if (total_len >= buf_size) {
		return ESP_FAIL;
	}

	while (cur_len < total_len) {
		int received =
			httpd_req_recv(req, buffer + cur_len, total_len - cur_len);
		if (received <= 0) {
			return ESP_FAIL;
		}
		cur_len += (size_t)received;
	}
	buffer[total_len] = '\0';
	return ESP_OK;
}

static esp_err_t send_ok_json(httpd_req_t *req) {
	httpd_resp_set_type(req, "application/json");
	return httpd_resp_send(req, OK_PAYLOAD, HTTPD_RESP_USE_STRLEN);
}

void web_handler_init(web_handler_t *self, lcd_service_t *lcd_svc,
					  nvs_service_t *nvs_svc, wifi_service_t *wifi_svc,
					  const char *base_path) {
	self->lcd_svc = lcd_svc;
	self->nvs_svc = nvs_svc;
	self->wifi_svc = wifi_svc;
	strlcpy(self->base_path, base_path, sizeof(self->base_path));
}

void web_handler_deinit(web_handler_t *self) {
	self->lcd_svc = NULL;
	self->nvs_svc = NULL;
	self->wifi_svc = NULL;
}

esp_err_t web_handler_healthcheck(web_handler_t *self, httpd_req_t *req) {
	(void)self;
	return send_ok_json(req);
}

esp_err_t web_handler_login(web_handler_t *self, httpd_req_t *req) {
	char buffer[1024] = {0};
	esp_err_t err = parse_buffer(req, buffer, sizeof(buffer));
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "login - Failed to parse buffer");
		httpd_resp_send_500(req);
		return err;
	}

	cJSON *root = cJSON_Parse(buffer);
	if (root == NULL) {
		httpd_resp_send_500(req);
		return ESP_FAIL;
	}

	const cJSON *ssid_item = cJSON_GetObjectItem(root, "ssid");
	const cJSON *password_item = cJSON_GetObjectItem(root, "password");
	if (!cJSON_IsString(ssid_item) || ssid_item->valuestring == NULL ||
		!cJSON_IsString(password_item) || password_item->valuestring == NULL) {
		cJSON_Delete(root);
		httpd_resp_send_500(req);
		return ESP_FAIL;
	}

	const char *ssid = ssid_item->valuestring;
	const char *password = password_item->valuestring;
	ESP_LOGI(TAG, "Received ssid: %s", ssid);

	// include the null terminator
	nvs_service_write_blob(self->nvs_svc, "wifi", "ssid", ssid,
							strlen(ssid) + 1);
	nvs_service_write_blob(self->nvs_svc, "wifi", "password", password,
							strlen(password) + 1);

	cJSON_Delete(root);
	return send_ok_json(req);
}

// Serve static files from the filesystem.
// Adapted from (https://tinyurl.com/mwp4tjtd)
esp_err_t web_handler_serve_static_files(web_handler_t *self,
										 httpd_req_t *req) {
	char filepath[FILE_PATH_MAX];

	strlcpy(filepath, self->base_path, sizeof(filepath));
	size_t uri_len = strlen(req->uri);
	if (uri_len > 0 && req->uri[uri_len - 1] == '/') {
		strlcat(filepath, "/index.html", sizeof(filepath));
	} else {
		strlcat(filepath, req->uri, sizeof(filepath));
	}

	int fd = open(filepath, O_RDONLY, 0);
	if (fd == -1) {
		// Asset not found: fall back to index.html so the SPA router can
		// handle client-side routes (e.g. /settings).
		strlcpy(filepath, self->base_path, sizeof(filepath));
		strlcat(filepath, "/index.html", sizeof(filepath));
		fd = open(filepath, O_RDONLY, 0);
	}
	if (fd == -1) {
		// index.html itself is missing - the LittleFS partition was likely
		// not flashed. Run 'idf.py flash' (not 'app-flash').
		ESP_LOGE(TAG, "Failed to open file : %s", filepath);
		httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
		return ESP_FAIL;
	}

	set_content_type_from_file(req, filepath);

	char *chunk = self->scratch;
	ssize_t read_bytes;
	do {
		read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
		if (read_bytes == -1) {
			ESP_LOGE(TAG, "Failed to read file : %s", filepath);
		} else if (read_bytes > 0) {
			if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
				close(fd);
				ESP_LOGE(TAG, "File sending failed!");
				httpd_resp_sendstr_chunk(req, NULL);
				httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
									"Failed to send file");
				return ESP_FAIL;
			}
		}
	} while (read_bytes > 0);
	close(fd);
	// An empty chunk signals the end of the response.
	httpd_resp_send_chunk(req, NULL, 0);
	ESP_LOGI(TAG, "File sending complete");
	return ESP_OK;
}

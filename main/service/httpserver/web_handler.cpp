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

WebHandler::WebHandler(ILcdService &lcd_svc, INvsService &nvs_svc,
					   IWifiService &wifi_svc) :
	lcd_svc_(lcd_svc), nvs_svc_(nvs_svc), wifi_svc_(wifi_svc) {
	ESP_LOGI("Handler", "lcd_svc  @ %p", static_cast<void *>(&lcd_svc_));
	ESP_LOGI("Handler", "nvs_svc  @ %p", static_cast<void *>(&nvs_svc_));
	ESP_LOGI("Handler", "wifi_svc @ %p", static_cast<void *>(&wifi_svc_));
}

esp_err_t WebHandler::healthcheck(httpd_req_t *req) {
	const etl::string<128> payload = R"({"message": "OK"})";
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, payload.c_str(), payload.length());
	return ESP_OK;
}

// Serve static files from filesystem
// Copied from
// https://github.com/espressif/esp-idf/blob/master/examples/protocols/http_server/restful_server/main/rest_server.c
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
		ESP_LOGE(TAG, "Failed to open file : %s", filepath);
		/* Respond with 500 Internal Server Error */
		httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
							"Failed to read existing file");
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
	ESP_LOGI(TAG, "File sending complete");
	/* Respond with an empty chunk to signal HTTP response completion */
	httpd_resp_send_chunk(req, NULL, 0);
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

//
// Created by eugen on 7/11/2026.
//

#ifndef CPP_STARTER_WEB_HANDLER_HPP
#define CPP_STARTER_WEB_HANDLER_HPP

#include "domain/interface/ildc_service.hpp"
#include "domain/interface/invs_service.hpp"
#include "domain/interface/iwifi_service.hpp"
#include <esp_http_server.h>
#include <esp_vfs.h>

#define SCRATCH_BUFSIZE (10240)
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)

namespace svc::httpserver {
typedef struct rest_server_context {
	char base_path[ESP_VFS_PATH_MAX + 1];
	char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

class WebHandler {
private:
	ILcdService &lcd_svc_;
	INvsService &nvs_svc_;
	IWifiService &wifi_svc_;
	static esp_err_t set_content_type_from_file(httpd_req_t *req,
												const char *filepath);

public:
	WebHandler(ILcdService &lcd_svc, INvsService &nvs_svc,
			   IWifiService &wifi_svc);

	esp_err_t healthcheck(httpd_req_t *req);
	static esp_err_t serve_static_files(httpd_req_t *req);
};
}; // namespace svc::httpserver

#endif // CPP_STARTER_WEB_HANDLER_HPP

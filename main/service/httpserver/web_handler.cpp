//
// Created by eugen on 7/11/2026.
//

#include "web_handler.hpp"
#include <cJSON.h>
#include <etl/string.h>

#include <esp_log.h>

using namespace svc::httpserver;

WebHandler::WebHandler(ILcdService &lcd_svc, INvsService &nvs_svc,
					   IWifiService &wifi_svc) :
	lcd_svc_(lcd_svc), nvs_svc_(nvs_svc), wifi_svc_(wifi_svc) {
	ESP_LOGI("Handler", "lcd_svc  @ %p", static_cast<void *>(&lcd_svc_));
	ESP_LOGI("Handler", "nvs_svc  @ %p", static_cast<void *>(&nvs_svc_));
	ESP_LOGI("Handler", "wifi_svc @ %p", static_cast<void *>(&wifi_svc_));
}

esp_err_t WebHandler::httpserver_healthcheck(httpd_req_t *req) {
	etl::string<128> payload = R"({"message": "OK"})";
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, payload.c_str(), payload.length());
	return ESP_OK;
}

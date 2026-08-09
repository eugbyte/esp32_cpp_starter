//
// Created by eugen on 8/9/2026.
//

#include "wifi_task.hpp"

#include "service/wifi/wifi_service.hpp"

#include <esp_log.h>

using namespace svc::wifi;

esp_err_t task::wifi_task(WifiService &wifi_service) {
	// esp_err_t err = wifi_service.sta_connect("ssid", "pwd");
	// if (err != ESP_OK) {
	// 	ESP_LOGE("wifi_task", "Failed to connect to WiFi: %s",
	// 			 esp_err_to_name(err));
	// }
	wifi_service.ap_connect();

	return ESP_OK;
}
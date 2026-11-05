//
// Native C counterpart of wifi_task.cpp.
//

#include "wifi_task.h"

#include <esp_log.h>

void wifi_task_init(wifi_task_t *self, wifi_service_t *wifi_service) {
	self->wifi_service = wifi_service;
}

void wifi_task_deinit(wifi_task_t *self) { self->wifi_service = NULL; }

esp_err_t wifi_task_start(wifi_task_t *self) {
	// esp_err_t err =
	// 	wifi_service_sta_connect(self->wifi_service, "ssid", "pwd");
	// if (err != ESP_OK) {
	// 	ESP_LOGE("wifi_task", "Failed to connect to WiFi: %s",
	// 			 esp_err_to_name(err));
	// }
	return wifi_service_ap_connect(self->wifi_service);
}

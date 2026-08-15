#include "service/httpserver/http_server.hpp"
#include "service/httpserver/web_handler.hpp"
#include "service/i2c/i2c.hpp"
#include "service/lcd/lcd_service.hpp"
#include "service/sensor/bmp280_service.hpp"
#include "service/sensor/ens160_service.hpp"
#include "service/storage/fs_service.hpp"
#include "service/storage/nvs_service.hpp"
#include "service/wifi/wifi_service.hpp"
#include "task/http_task.hpp"
#include "task/i2c_sensor_task.hpp"
#include "task/wifi_task.hpp"

#include <cstdio>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_random.h>
#include <etl/span.h>
#include <etl/string.h>
#include <mqtt_client.h>
#include <nvs_flash.h>

// Entry point: brings up NVS, the default event loop and netif, then the
// LCD, Wi-Fi (STA + AP), NVS-backed storage, filesystem and HTTP server
// services, before looping to show a random number on the LCD.
extern "C" void app_main(void) {
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(esp_netif_init());
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
		err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);
	printf("Finished initializing native esp-idf services\n");

	auto nvs_service = svc::storage::NvsService();
	auto lcd_service = svc::lcd::LcdService();
	auto wifi_service = svc::wifi::WifiService();
	auto i2c_service = svc::i2c::I2CService();
	auto httpserver = svc::httpserver::HttpServer();
	auto bmp280_service = svc::sensor::Bmp280Service(i2c_service);
	auto ens160_service = svc::sensor::Ens160Service(i2c_service);
	auto web_handler =
		svc::httpserver::WebHandler(lcd_service, nvs_service, wifi_service);

	ESP_ERROR_CHECK(task::http_task(web_handler, httpserver));
	ESP_ERROR_CHECK(task::wifi_task(wifi_service));
	ESP_ERROR_CHECK(task::i2c_sensor_task(bmp280_service));

	while (true) {
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

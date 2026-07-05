#include "service/lcd/lcd_service.hpp"
#include "service/wifi/wifi_service.hpp"
#include <esp_log.h>
#include <esp_random.h>
#include <etl/string.h>
#include <mqtt_client.h>

#include <cstdio>
#include <esp_netif.h>
#include <nvs_flash.h>

extern "C" void app_main(void) {
	esp_err_t err = {0};

	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(esp_netif_init());

	printf("Initialized native esp-idf services\n");

	auto lcd_service = svc::lcd::LcdService();
	lcd_service.clear();
	lcd_service.send_text("LCD initialized");

	auto wifi_service = svc::wifi::WifiService();
	etl::string<32> ssid = "Tham Network";
	etl::string<32> password = "28Stratton";
	if (err = wifi_service.sta_connect(ssid, password);
		err != ESP_OK) {
		lcd_service.clear();
		lcd_service.send_text("Wifi connection failed");
	} else {
		lcd_service.clear();
		lcd_service.send_text("Wifi connected");
	}

	etl::string<32> buffer = "";

	while (true) {
		const uint32_t random_num = (esp_random() % 10) + 1;
		sprintf(buffer.data(), "num: %d", (int)random_num);
		ESP_LOGI("main", "num: %d", random_num);
		lcd_service.clear();
		lcd_service.send_text(buffer.data());

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

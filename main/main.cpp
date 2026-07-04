#include "esp_log.h"
#include "esp_random.h"
#include "etl/string.h"
#include "lcd/LcdService.hpp"
#include "mqtt_client.h"
#include <cstdio>
#include <esp_netif.h>
#include <nvs_flash.h>

extern "C" void app_main(void) {
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(esp_netif_init());

	printf("Initialized native esp-idf services\n");

	auto lcd_service = LcdService();
	lcd_service.Clear();
	lcd_service.SendText("Device initialized");

	etl::string<20> buffer = "";

	while (true) {
		const uint32_t random_num = (esp_random() % 10) + 1;
		sprintf(buffer.data(), "num: %d", (int)random_num);
		ESP_LOGI("main", "num: %d", random_num);
		lcd_service.Clear();
		lcd_service.SendText(buffer.data());

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

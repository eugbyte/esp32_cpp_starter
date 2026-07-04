#include "esp_log.h"
#include "esp_random.h"
#include "etl/string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd/LcdService.hpp"
#include "mqtt_client.h"
#include <cstdio>

extern "C" void app_main(void) {
	printf("Hello world!\n");

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

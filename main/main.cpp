#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd/LcdService.h"
#include <cstdio>
#include "mqtt_client.h"

extern "C" void app_main(void) {
	printf("Hello world!\n");

	LcdService lcd_service = LcdService();
	lcd_service.Clear();
	lcd_service.SendText("Device initialized");

	while (true) {
		const uint32_t random_num = (esp_random() % 10) + 1;
		char buffer[20];
		sprintf(buffer, "num: %d", (int)random_num);
		ESP_LOGI("main", "num: %d", random_num);
		lcd_service.Clear();
		lcd_service.SendText(buffer);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

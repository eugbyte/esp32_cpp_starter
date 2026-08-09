//
// Created by eugen on 8/9/2026.
//

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include "i2c_sensor_task.hpp"

#include <esp_log.h>

using namespace svc::bmp280_sensor;

esp_err_t task::i2c_sensor_task(Bmp280Service &bmp280_service) {
	esp_err_t err = bmp280_service.connect();
	if (err != ESP_OK) {
		ESP_LOGE("main", "Failed to initialize BMP280 service");
		return err;
	}

	auto handler = [](void* pvParameters) -> void {
		auto* service = static_cast<Bmp280Service*>(pvParameters);
		int count = 0;

		while (true) {
			auto [temperature, temperature_err] = service->bmp280_read_temp();
			if (temperature_err != ESP_OK) {
				ESP_LOGE("main", "Failed to read temperature from BMP280");
				continue;
			}

			auto [pressure, pressure_err] = service->bmp280_read_pressure();
			if (pressure_err != ESP_OK) {
				ESP_LOGE("main", "Failed to read pressure from BMP280");
				continue;
			}

			if (count < 10) {
				ESP_LOGI("main", "Temperature: %.2f, Pressure: %.2f", temperature, pressure);
				count += 1;
			}

			vTaskDelay(pdMS_TO_TICKS(1000));
		}
	};

	xTaskCreatePinnedToCore(handler, "i2c_sensor_task", I2C_SENSOR_STACK_SIZE, &bmp280_service, I2C_SENSOR_TASK_PRIORITY, nullptr, I2C_SENSOR_CORE_ID);
	return ESP_OK;
}

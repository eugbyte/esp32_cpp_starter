//
// Created by eugen on 8/9/2026.
//

// freeRTOS must be included before any other header files
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
// ---
#include "i2c_sensor_task.hpp"
#include <esp_log.h>

static auto *TAG_TASK = "i2c_sensor_task";

esp_err_t task::i2c_sensor_task(i2c_sensor_services_t &services) {
	esp_err_t err = services.bmp280_service->connect();
	if (err != ESP_OK) {
		ESP_LOGE(TAG_TASK, "Failed to initialize BMP280 service");
		return err;
	}

	auto [ambient_temp, ambient_temp_err] =
		services.bmp280_service->read_temp();
	if (ambient_temp_err != ESP_OK) {
		ESP_LOGE(TAG_TASK, "Failed to read temperature from BMP280");
		return ambient_temp_err;
	}
	err = services.ens160_service->connect(&ambient_temp, nullptr);
	if (err != ESP_OK) {
		ESP_LOGE(TAG_TASK, "Failed to initialize Ens160 service");
		return err;
	}

	auto handler = [](void *pvParameters) -> void {
		auto *service_ptr = static_cast<i2c_sensor_services_t *>(pvParameters);
		i2c_sensor_services_t &services = *service_ptr;

		while (true) {
			auto [temperature, temperature_err] =
				services.bmp280_service->read_temp();
			if (temperature_err != ESP_OK) {
				ESP_LOGE(TAG_TASK, "Failed to read temperature from BMP280");
				continue;
			}

			auto [pressure, pressure_err] =
				services.bmp280_service->read_pressure();
			if (pressure_err != ESP_OK) {
				ESP_LOGE(TAG_TASK, "Failed to read pressure from BMP280");
				continue;
			}

			ESP_LOGI(TAG_TASK, "Temperature: %.2f C, Pressure: %.2f hPa",
					 temperature, pressure);

			auto [air_info, air_info_err] =
				services.ens160_service->read_air_data();
			if (air_info_err != ESP_OK) {
				ESP_LOGE(TAG_TASK, "Failed to read air info from Ens160");
				continue;
			}

			ESP_LOGI(TAG_TASK, "ens160_read_data: agi=%d, tvoc=%d, eco2=%d, etoh=%d", air_info.agi_uba, air_info.tvoc_ppb, air_info.eco2_ppm, air_info.etoh_ppb);

			vTaskDelay(pdMS_TO_TICKS(5000));
		}
	};

	xTaskCreatePinnedToCore(handler, "i2c_sensor_task", I2C_SENSOR_STACK_SIZE,
							&services, I2C_SENSOR_TASK_PRIORITY, nullptr,
							I2C_SENSOR_CORE_ID);
	return ESP_OK;
}

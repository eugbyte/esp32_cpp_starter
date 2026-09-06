//
// Created by eugen on 9/5/2026.
//

// freeRTOS must be included before any other header files
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
// ---
#include "spi_sensor_task.hpp"
#include <esp_log.h>

static auto *TAG_TASK = "spi_sensor_task";

esp_err_t task::spi_sensor_task(IEns160Service &ens160_service) {
	// No ambient sensor on the SPI bus, so skip temperature/humidity
	// compensation
	esp_err_t err = ens160_service.connect(nullptr, nullptr);
	if (err != ESP_OK) {
		ESP_LOGE(TAG_TASK, "Failed to initialize Ens160 service");
		return err;
	}

	auto handler = [](void *pvParameters) -> void {
		auto *service = static_cast<IEns160Service *>(pvParameters);

		while (true) {
			auto [air_info, air_info_err] = service->read_air_data();
			if (air_info_err != ESP_OK) {
				ESP_LOGE(TAG_TASK, "Failed to read air info from Ens160");
				continue;
			}

			ESP_LOGI(TAG_TASK,
					 "ens160_read_data: agi=%d (UBA), tvoc=%d ppb, eco2=%d "
					 "ppm, etoh=%d ppb",
					 air_info.agi_uba, air_info.tvoc_ppb, air_info.eco2_ppm,
					 air_info.etoh_ppb);

			vTaskDelay(pdMS_TO_TICKS(5000));
		}
	};

	xTaskCreatePinnedToCore(handler, "spi_sensor_task", SPI_SENSOR_STACK_SIZE,
							&ens160_service, SPI_SENSOR_TASK_PRIORITY, nullptr,
							SPI_SENSOR_CORE_ID);
	return ESP_OK;
}

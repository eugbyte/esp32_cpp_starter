//
// Native C counterpart of spi_sensor_task.cpp.
//

#include "spi_sensor_task.h"

#include <esp_log.h>

#include <stdbool.h>

static const char TAG[] = "spi_sensor_task";

// Reads and logs the sensor once.
static void poll_once(spi_sensor_task_t *self) {
	ens_160_read_info_t air_info = {0};
	if (ens160_spi_service_read_air_data(self->ens160_service, &air_info) !=
		ESP_OK) {
		ESP_LOGE(TAG, "Failed to read air info from Ens160");
		return;
	}
	ESP_LOGI(TAG,
			 "ens160_read_data: agi=%d (UBA), tvoc=%d ppb, eco2=%d ppm, "
			 "etoh=%d ppb",
			 air_info.agi_uba, air_info.tvoc_ppb, air_info.eco2_ppm,
			 air_info.etoh_ppb);
}

static void task_loop(void *parameter) {
	spi_sensor_task_t *self = (spi_sensor_task_t *)parameter;
	while (true) {
		poll_once(self);
		// delay on errors too, so a failing sensor doesn't spin the core
		vTaskDelay(pdMS_TO_TICKS(SPI_SENSOR_POLL_MS));
	}
}

void spi_sensor_task_init(spi_sensor_task_t *self,
						  ens160_spi_service_t *ens160_service) {
	self->ens160_service = ens160_service;
	self->task_handle = NULL;
}

void spi_sensor_task_deinit(spi_sensor_task_t *self) {
	if (self->task_handle != NULL) {
		vTaskDelete(self->task_handle);
		self->task_handle = NULL;
	}
}

esp_err_t spi_sensor_task_start(spi_sensor_task_t *self) {
	// No ambient sensor on the SPI bus, so skip compensation
	esp_err_t err = ens160_spi_service_connect(self->ens160_service, NULL, NULL);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to initialize Ens160 service");
		return err;
	}

	BaseType_t ok = xTaskCreatePinnedToCore(
		task_loop, "spi_sensor_task", SPI_SENSOR_STACK_SIZE, self,
		SPI_SENSOR_TASK_PRIORITY, &self->task_handle, SPI_SENSOR_CORE_ID);
	return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

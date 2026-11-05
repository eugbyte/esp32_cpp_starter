//
// Native C counterpart of i2c_sensor_task.cpp.
//

#include "i2c_sensor_task.h"

#include <esp_log.h>

#include <stdbool.h>

static const char TAG[] = "i2c_sensor_task";

// Reads and logs both sensors once.
static void poll_once(i2c_sensor_task_t *self) {
	bmp_280_read_info_t thermal = {0};
	if (bmp280_service_read_thermal(self->bmp280_service, &thermal) !=
		ESP_OK) {
		ESP_LOGE(TAG, "Failed to read thermal data from BMP280");
		return;
	}
	ESP_LOGI(TAG, "Temperature: %d C, Pressure: %d hPa",
			 thermal.temperature_celcius, thermal.relative_humidity);

	ens_160_read_info_t air_info = {0};
	if (ens160_i2c_service_read_air_data(self->ens160_service, &air_info) !=
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
	i2c_sensor_task_t *self = (i2c_sensor_task_t *)parameter;
	while (true) {
		poll_once(self);
		// delay on errors too, so a failing sensor doesn't spin the core
		vTaskDelay(pdMS_TO_TICKS(I2C_SENSOR_POLL_MS));
	}
}

void i2c_sensor_task_init(i2c_sensor_task_t *self,
						  bmp280_service_t *bmp280_service,
						  ens160_i2c_service_t *ens160_service) {
	self->bmp280_service = bmp280_service;
	self->ens160_service = ens160_service;
	self->task_handle = NULL;
}

void i2c_sensor_task_deinit(i2c_sensor_task_t *self) {
	if (self->task_handle != NULL) {
		vTaskDelete(self->task_handle);
		self->task_handle = NULL;
	}
}

esp_err_t i2c_sensor_task_start(i2c_sensor_task_t *self) {
	esp_err_t err = bmp280_service_connect(self->bmp280_service);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to initialize BMP280 service");
		return err;
	}

	bmp_280_read_info_t ambient = {0};
	err = bmp280_service_read_thermal(self->bmp280_service, &ambient);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to read temperature from BMP280");
		return err;
	}

	float ambient_temp = ambient.temperature_celcius;
	err = ens160_i2c_service_connect(self->ens160_service, &ambient_temp, NULL);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to initialize Ens160 service");
		return err;
	}

	BaseType_t ok = xTaskCreatePinnedToCore(
		task_loop, "i2c_sensor_task", I2C_SENSOR_STACK_SIZE, self,
		I2C_SENSOR_TASK_PRIORITY, &self->task_handle, I2C_SENSOR_CORE_ID);
	return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

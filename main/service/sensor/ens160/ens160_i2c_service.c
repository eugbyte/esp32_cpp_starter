//
// Native C counterpart of ens160_i2c_service.cpp.
//

#include "ens160_i2c_service.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include <string.h>

static const char TAG[] = "ens160";

// Resets the device, then switches to standard (continuous) measurement
// and standard power mode.
static esp_err_t set_normal_mode(ens160_i2c_service_t *self) {
	uint8_t reset_buffer[2] = {ENS160_OP_MODE_ADDR, ENS160_OPMODE_RESET};
	esp_err_t err = i2c_service_write_buffer(
		self->i2c_svc, self->device_handle, reset_buffer, sizeof(reset_buffer));
	if (err != ESP_OK) {
		return err;
	}
	vTaskDelay(pdMS_TO_TICKS(20));

	uint8_t write_buf[2] = {ENS160_OP_MODE_ADDR, ENS160_NORMAL_MODE};
	err = i2c_service_write_buffer(self->i2c_svc, self->device_handle,
									write_buf, sizeof(write_buf));
	vTaskDelay(pdMS_TO_TICKS(20));
	return err;
}

// Writes temperature and humidity compensation values (s 16.2.5 -
// s 16.2.6). Either pointer may be NULL to skip it.
static esp_err_t set_compensation_values(ens160_i2c_service_t *self,
										 const float *temp_celcius_opt,
										 const float *relative_humidity_opt) {
	if (temp_celcius_opt != NULL) {
		uint8_t buffer[3] = {0};
		ens160_to_temp_buffer(ENS160_TEMP_ADDR, buffer, *temp_celcius_opt);
		i2c_service_write_buffer(self->i2c_svc, self->device_handle, buffer,
								  sizeof(buffer));
	}

	if (relative_humidity_opt != NULL) {
		uint8_t buffer[3] = {0};
		ens160_to_humidity_buffer(ENS160_HUMIDITY_ADDR, buffer,
								  *relative_humidity_opt);
		i2c_service_write_buffer(self->i2c_svc, self->device_handle, buffer,
								  sizeof(buffer));
	}
	return ESP_OK;
}

void ens160_i2c_service_init(ens160_i2c_service_t *self,
							 i2c_service_t *i2c_svc) {
	memset(self, 0, sizeof(*self));
	self->i2c_svc = i2c_svc;
}

void ens160_i2c_service_deinit(ens160_i2c_service_t *self) {
	if (!self->subscribed) {
		return;
	}
	esp_err_t err =
		i2c_service_unsubscribe(self->i2c_svc, &self->device_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to unsubscribe ens160 device handle");
	}
	self->subscribed = false;
}

esp_err_t
ens160_i2c_service_connect(ens160_i2c_service_t *self,
						   const float *ambient_temp_celcius_opt,
						   const float *ambient_relative_humidity_opt) {
	esp_err_t err = i2c_service_subscribe(self->i2c_svc, ENS160_ADDR,
										   &self->device_handle);
	if (err != ESP_OK) {
		return err;
	}
	self->subscribed = true;

	uint8_t data[2] = {7, 7};
	i2c_service_read(self->i2c_svc, self->device_handle, ENS160_REG_ID, data,
					  sizeof(data));
	ESP_LOGI(TAG, "WHO_AM_I = %X%X", data[0], data[1]);

	err = set_normal_mode(self);
	if (err != ESP_OK) {
		return err;
	}
	return set_compensation_values(self, ambient_temp_celcius_opt,
								   ambient_relative_humidity_opt);
}

esp_err_t ens160_i2c_service_read_air_data(ens160_i2c_service_t *self,
										   ens_160_read_info_t *out_info) {
	// 5 contiguous bytes = 1 (AGI) + 2 (TVOC) + 2 (ECO2), s 16.2.8 - 16.2.10
	uint8_t data[5] = {0};
	esp_err_t err = i2c_service_read(self->i2c_svc, self->device_handle,
									  ENS160_AQI_REG, data, sizeof(data));
	if (err != ESP_OK) {
		memset(out_info, 0, sizeof(*out_info));
		return err;
	}

	*out_info = ens160_to_read_info(data);
	return ESP_OK;
}

//
// Created by eugen on 8/15/2026.
//

// freeRTOS must be included before any other header files
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
//
#include "ens160_service.hpp"
#include <FreeRTOSConfig.h>
#include <esp_log.h>
#include <freertos/projdefs.h>
#include <math.h>

using namespace svc::sensor;

Ens160Service::Ens160Service(II2CService &i2c_service) :
	i2c_svc_(i2c_service) {}

Ens160Service::~Ens160Service() {
	esp_err_t err = i2c_svc_.unsubscribe(&ens160_device_handle_);
	if (err != ESP_OK) {
		ESP_LOGE("ens160", "Failed to unsubscribe ens160 device handle");
	}
}

esp_err_t Ens160Service::connect(const float *ambient_temp_celcius_opt,
								 const float *ambient_relative_humidity_opt) {
	uint8_t data[2] = {7, 7};
	esp_err_t err = i2c_svc_.subscribe(ENS160_ADDR, &ens160_device_handle_);
	if (err != ESP_OK) {
		return err;
	}
	i2c_svc_.read(ens160_device_handle_, ENS160_REG_ID, data, sizeof(data));
	// address should be 0x00
	ESP_LOGI("ens_160", "WHO_AM_I = %X%X", data[0], data[1]);
	err = set_normal_mode();
	if (err != ESP_OK) {
		return err;
	}
	return set_compensation_values(ambient_temp_celcius_opt,
								   ambient_relative_humidity_opt);
}

esp_err_t Ens160Service::set_normal_mode() const {
	// reset the device
	esp_err_t err = {};
	// esp_err_t err = i2c_svc_.write(ens160_device_handle_,
	// ENS160_OP_MODE_ADDR, 							   ENS160_OPMODE_RESET);
	// if (err != ESP_OK) { 	return err;
	// }
	// vTaskDelay(pdMS_TO_TICKS(20));

	// Switch to standard (continuous) measurement mode and standard power mode
	uint8_t write_buf[2] = {ENS160_OP_MODE_ADDR, ENS160_NORMAL_MODE};
	err = i2c_svc_.write_buffer(ens160_device_handle_, write_buf,
								sizeof(write_buf));
	vTaskDelay(pdMS_TO_TICKS(20));
	return err;
}

etl::tuple<ens_160_read_info_t, esp_err_t> Ens160Service::read_air_data() {

	ens_160_read_info_t data = {};
	ens160_read_data(&data.agi_uba, &data.tvoc_ppb, &data.eco2_ppm,
					 &data.etoh_ppb);
	return {data, ESP_OK};
}

esp_err_t Ens160Service::set_compensation_values(
	const float *temp_celcius_opt, const float *relative_humidity_opt) const {
	// write temperature and humidity as compensation values (s 16.2.5 -
	// s 16.2.6)
	uint16_t temperature = 0;
	if (temp_celcius_opt != nullptr) {
		temperature = (*temp_celcius_opt + 273.15f) * 64.0f;
		uint8_t buffer[3] = {};
		buffer[0] = ENS160_TEMP_ADDR;
		buffer[1] = temperature & 0b11111111; // LSB
		buffer[2] = temperature >> 8;		  // MSB
		i2c_svc_.write_buffer(ens160_device_handle_, buffer, sizeof(buffer));
	}

	uint16_t humidity = 0;
	if (relative_humidity_opt != nullptr) {
		humidity = (*relative_humidity_opt) * 512.0f;
		uint8_t buffer[3] = {};
		buffer[0] = ENS160_HUMIDITY_ADDR;
		buffer[1] = humidity & 0b11111111; // LSB
		buffer[2] = humidity >> 8;		   // MSB
		i2c_svc_.write_buffer(ens160_device_handle_, buffer, sizeof(buffer));
	}
	return ESP_OK;
}

// agi - Air Quality Index, UBA
// tvoc - Total Volatile Organic Compounds in ppb
// eco2 - Carbon Dioxide Equivalent in ppm
// etoh - ethanol concentration in ppb
esp_err_t Ens160Service::ens160_read_data(int8_t *agi, int16_t *tvoc,
										  int16_t *eco2, int16_t *etoh) {
	// 5 bytes of contiguous data, s 16.2.8 - 16.2.10
	// 5 = 1 (AGI) + 2 (TVOC) + 2 (ECO2); ETOH mirrors TVOC at 0x22
	uint8_t data[5] = {};

	esp_err_t err =
		i2c_svc_.read(ens160_device_handle_, ENS160_AQI_REG, data, sizeof(data));
	if (err != ESP_OK) {
		return err;
	}

	ESP_LOGI("ens_160", "ens160_read_data: 0=%d, 1=%d, 2=%d, 3=%d, 4=%d", data[0], data[1], data[2], data[3], data[4]);

	*agi = data[0] & 0b111;
	*tvoc = (data[2] << 8) | data[1];
	*eco2 = (data[4] << 8) | data[3];
	*etoh = *tvoc;
	return err;
}
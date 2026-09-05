//
// Created by eugen on 9/4/2026.
//

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include "ens160_spi_service.hpp"
#include <esp_log.h>
#include <freertos/projdefs.h>

using namespace svc::sensor::ens160;

Ens160Service_SPI::Ens160Service_SPI(ISPIService &spi) : spi_svc_(spi) {}

Ens160Service_SPI::~Ens160Service_SPI() {
	spi_svc_.unsubscribe(ens160_device_handle_);
}

ens_160_read_info_t to_read_info(uint8_t data[5]) {
	ens_160_read_info_t result = {};
	ESP_LOGI("ens_160", "ens160_read_data: 0=%d, 1=%d, 2=%d, 3=%d, 4=%d",
			 data[0], data[1], data[2], data[3], data[4]);

	result.agi_uba = data[0] & 0b111;
	result.tvoc_ppb = (data[2] << 8) | data[1];
	result.eco2_ppm = (data[4] << 8) | data[3];
	result.etoh_ppb = result.eco2_ppm;
	return result;
}

etl::tuple<ens_160_read_info_t, esp_err_t> Ens160Service_SPI::read_air_data() {
	// 5 bytes of contiguous data, s 16.2.8 - 16.2.10
	// 5 = 1 (AGI) + 2 (TVOC) + 2 (ECO2); ETOH mirrors TVOC at 0x22
	// In SPI, first byte is register address
	// 6 = addr + 5
	constexpr int len = 1 + 5;
	uint8_t rx_data[len] = {};
	uint8_t tx_data[len] = {};
	size_t bit_size = sizeof(rx_data) * 8;

	// s 14.2.2, s 14.2.3
	tx_data[0] = (ENS160_AQI_REG << 1) | ENS160_READ_BIT;

	esp_err_t err = spi_svc_.spi_read_write_byte(ens160_device_handle_, rx_data,
												 tx_data, bit_size);
	if (err != ESP_OK) {
		return {ens_160_read_info_t{}, err};
	}

	uint8_t data[5] = {};
	memcpy(data, &rx_data[1], sizeof(data)); // skip only the addr-byte slot
	return {to_read_info(data), err};
}

esp_err_t
Ens160Service_SPI::connect(const float *ambient_temp_celcius_opt,
						   const float *ambient_relative_humidity_opt) {
	uint8_t rx_data[3] = {7, 7, 7};
	uint8_t tx_data[3] = {};
	size_t bit_size = sizeof(rx_data) * 8;

	spi_device_interface_config_t devcfg =
		spi_svc_.create_default_device_config(PIN_NUM_CS);
	esp_err_t err = spi_svc_.subscribe(&ens160_device_handle_, devcfg);
	if (err != ESP_OK) {
		return err;
	}

	tx_data[0] = (ENS160_REG_ID << 1) | ENS160_READ_BIT;

	err = spi_svc_.spi_read_write_byte(ens160_device_handle_, rx_data, tx_data,
									   bit_size);
	if (err != ESP_OK) {
		return err;
	}
	// address should be 0x00
	ESP_LOGI("ens_160", "WHO_AM_I = %X%X", rx_data[1], rx_data[2]);
	err = set_normal_mode();
	if (err != ESP_OK) {
		return err;
	}
	return set_compensation_values(ambient_temp_celcius_opt,
								   ambient_relative_humidity_opt);
}

esp_err_t Ens160Service_SPI::set_normal_mode() const {
	// reset the device
	uint8_t tx_data[2] = {};
	size_t bit_size = sizeof(tx_data) * 8;
	tx_data[0] = (ENS160_OP_MODE_ADDR << 1) | ENS160_WRITE_BIT;
	tx_data[1] = ENS160_OPMODE_RESET;

	esp_err_t err = spi_svc_.spi_read_write_byte(ens160_device_handle_, nullptr,
												 tx_data, bit_size);
	if (err != ESP_OK) {
		return err;
	}
	vTaskDelay(pdMS_TO_TICKS(20));

	// Switch to standard (continuous) measurement mode and standard power mode
	tx_data[1] = ENS160_NORMAL_MODE;
	err = spi_svc_.spi_read_write_byte(ens160_device_handle_, nullptr, tx_data,
									   bit_size);
	if (err != ESP_OK) {
		return err;
	}
	vTaskDelay(pdMS_TO_TICKS(20));
	return err;
}

esp_err_t Ens160Service_SPI::set_compensation_values(
	const float *temp_celcius_opt, const float *relative_humidity_opt) const {
	// write temperature and humidity as compensation values (s 16.2.5 -
	// s 16.2.6)
	uint16_t temperature = 0;
	if (temp_celcius_opt != nullptr) {
		temperature = (*temp_celcius_opt + 273.15f) * 64.0f;
		uint8_t buffer[3] = {};
		buffer[0] = (ENS160_TEMP_ADDR << 1) | ENS160_WRITE_BIT;
		buffer[1] = temperature & 0b11111111; // LSB
		buffer[2] = temperature >> 8;		  // MSB
		spi_svc_.spi_read_write_byte(ens160_device_handle_, nullptr, buffer,
									 sizeof(buffer) * 8);
	}

	uint16_t humidity = 0;
	if (relative_humidity_opt != nullptr) {
		humidity = (*relative_humidity_opt) * 512.0f;
		uint8_t buffer[3] = {};
		buffer[0] = (ENS160_HUMIDITY_ADDR << 1) | ENS160_WRITE_BIT;
		buffer[1] = humidity & 0b11111111; // LSB
		buffer[2] = humidity >> 8;		   // MSB
		spi_svc_.spi_read_write_byte(ens160_device_handle_, nullptr, buffer,
									 sizeof(buffer) * 8);
	}
	return ESP_OK;
}
//
// Created by eugen on 8/8/2026.
//

#include "bmp280_service.hpp"

#include "domain/interface/ii2c_service.hpp"

#include <esp_log.h>
#include <etl/tuple.h>

using namespace svc::bmp280_sensor;

Bmp280Service::Bmp280Service(II2CService &i2c_service) : i2c_svc_(i2c_service) {
	uint8_t data[2] = {};

	constexpr uint8_t bmp280_address = 0x76;
	i2c_service.subscribe(bmp280_address, &bmp280_device_handle_);
	constexpr uint8_t bmp280_reg_id =
		0xD0; // chip ID register; expected value 0x58
	i2c_service.read(bmp280_device_handle_, bmp280_reg_id, data, 1);
	ESP_LOGI("i2c", "WHO_AM_I = %X", data[0]);
}

etl::tuple<float, esp_err_t> Bmp280Service::bmp280_read_temp() {
	uint8_t data[3] = {};	// 3 bytes = msb + lsb + xlsb
	constexpr uint8_t temp_register_addr = 0xFA;
	esp_err_t err = i2c_svc_.read(bmp280_device_handle_, temp_register_addr, data,
				  sizeof(data));
	if (err != ESP_OK) {
		return {0.0f, err};
	}
	const uint8_t msb = static_cast<int32_t>(data[0]) << 12;
	const uint8_t lsb = static_cast<int32_t>(data[1]) << 4;
	const uint8_t xlsb = data[2] >> 4;
	int32_t adc_T = msb | lsb | xlsb;
	return {adc_T, ESP_OK};
}

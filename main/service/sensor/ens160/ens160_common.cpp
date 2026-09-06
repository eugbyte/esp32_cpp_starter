//
// Created by eugen on 9/4/2026.
//

#include "ens160_common.hpp"

#include <esp_log.h>

namespace svc::sensor::ens160 {

// agi - Air Quality Index, UBA
// tvoc - Total Volatile Organic Compounds in ppb
// eco2 - Carbon Dioxide Equivalent in ppm
// etoh - ethanol concentration in ppb; mirrors TVOC at 0x22
ens_160_read_info_t to_read_info(const uint8_t data[5]) {
	ens_160_read_info_t result = {};
	ESP_LOGI("ens_160", "ens160_read_data: 0=%d, 1=%d, 2=%d, 3=%d, 4=%d",
			 data[0], data[1], data[2], data[3], data[4]);

	result.agi_uba = data[0] & 0b111;
	result.tvoc_ppb = (data[2] << 8) | data[1];
	result.eco2_ppm = (data[4] << 8) | data[3];
	result.etoh_ppb = result.tvoc_ppb;
	return result;
}

// temperature compensation value, Kelvin * 64 (s 16.2.5)
uint16_t to_temp_value(float temp_celcius) {
	return (temp_celcius + 273.15f) * 64.0f;
}

// relative humidity compensation value, % * 512 (s 16.2.6)
uint16_t to_humidity_value(float relative_humidity) {
	return relative_humidity * 512.0f;
}

void to_temp_buffer(uint8_t temp_reg_addr, uint8_t buffer[3],
					float temp_celcius) {
	uint16_t temperature = to_temp_value(temp_celcius);
	buffer[0] = temp_reg_addr;
	buffer[1] = temperature & 0b11111111; // LSB
	buffer[2] = temperature >> 8;		  // MSB
}

void to_humidity_buffer(uint8_t humidity_reg_addr, uint8_t buffer[3],
						float relative_humidity) {
	uint16_t humidity = to_humidity_value(relative_humidity);
	buffer[0] = humidity_reg_addr;
	buffer[1] = humidity & 0b11111111; // LSB
	buffer[2] = humidity >> 8;		   // MSB
}

} // namespace svc::sensor::ens160

//
// Native C counterpart of ens160_common.cpp.
//

#include "ens160_common.h"

#include <esp_log.h>

// agi - Air Quality Index, UBA
// tvoc - Total Volatile Organic Compounds in ppb
// eco2 - Carbon Dioxide Equivalent in ppm
// etoh - ethanol concentration in ppb; mirrors TVOC at 0x22
ens_160_read_info_t ens160_to_read_info(const uint8_t data[5]) {
	ESP_LOGI("ens_160", "ens160_read_data: 0=%d, 1=%d, 2=%d, 3=%d, 4=%d",
			 data[0], data[1], data[2], data[3], data[4]);

	ens_160_read_info_t result = {0};
	result.agi_uba = (int8_t)(data[0] & 0b111);
	result.tvoc_ppb = (uint16_t)((data[2] << 8) | data[1]);
	result.eco2_ppm = (uint16_t)((data[4] << 8) | data[3]);
	result.etoh_ppb = result.tvoc_ppb;
	return result;
}

uint16_t ens160_to_temp_value(float temp_celcius) {
	return (uint16_t)((temp_celcius + 273.15f) * 64.0f);
}

uint16_t ens160_to_humidity_value(float relative_humidity) {
	return (uint16_t)(relative_humidity * 512.0f);
}

void ens160_to_temp_buffer(uint8_t temp_reg_addr, uint8_t buffer[3],
						   float temp_celcius) {
	uint16_t temperature = ens160_to_temp_value(temp_celcius);
	buffer[0] = temp_reg_addr;
	buffer[1] = (uint8_t)(temperature & 0xFF); // LSB
	buffer[2] = (uint8_t)(temperature >> 8);   // MSB
}

void ens160_to_humidity_buffer(uint8_t humidity_reg_addr, uint8_t buffer[3],
							   float relative_humidity) {
	uint16_t humidity = ens160_to_humidity_value(relative_humidity);
	buffer[0] = humidity_reg_addr;
	buffer[1] = (uint8_t)(humidity & 0xFF); // LSB
	buffer[2] = (uint8_t)(humidity >> 8);	// MSB
}

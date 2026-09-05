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

} // namespace svc::sensor::ens160

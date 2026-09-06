//
// Created by eugen on 9/4/2026.
//

#ifndef CPP_STARTER_ENS160_COMMON_HPP
#define CPP_STARTER_ENS160_COMMON_HPP

#include "domain/model/model.hpp"

#include <cstdint>

// The I²C slave address is 0x52 (when MISO/ADDR is set low via ground) or 0x53
// (MISO/ADDR is set high via pull-up). (s 14)
#define ENS160_ADDR 0x52
#define ENS160_REG_ID 0x60		 // s 16.2.1
#define ENS160_OP_MODE_ADDR 0x10 // s 16.2.2
#define ENS160_NORMAL_MODE 0x02	 // s 16.2.2
#define ENS160_AQI_REG 0x21		 // s 16.2.8
#define ENS160_TEMP_ADDR 0x13
#define ENS160_HUMIDITY_ADDR 0x15
#define ENS160_OPMODE_RESET 0xF0

namespace svc::sensor::ens160 {
ens_160_read_info_t to_read_info(const uint8_t data[5]);
uint16_t to_temp_value(float temp_celcius);
uint16_t to_humidity_value(float relative_humidity);
void to_temp_buffer(uint8_t temp_reg_addr, uint8_t buffer[3],
					float temp_celcius);
void to_humidity_buffer(uint8_t humidity_reg_addr, uint8_t buffer[3],
						float relative_humidity);
} // namespace svc::sensor::ens160

#endif // CPP_STARTER_ENS160_COMMON_HPP

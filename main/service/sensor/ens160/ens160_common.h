//
// Native C counterpart of ens160_common.hpp.
//
// Stateless, transport-independent ENS160 helpers shared by the I2C and SPI
// services.
//

#ifndef C_STARTER_ENS160_COMMON_H
#define C_STARTER_ENS160_COMMON_H

#include "domain/model/model.h"

#include <stdint.h>

// The I2C slave address is 0x52 (MISO/ADDR low) or 0x53 (MISO/ADDR high).
// (s 14)
#define ENS160_ADDR 0x52
#define ENS160_REG_ID 0x60		 // s 16.2.1
#define ENS160_OP_MODE_ADDR 0x10 // s 16.2.2
#define ENS160_NORMAL_MODE 0x02	 // s 16.2.2
#define ENS160_AQI_REG 0x21		 // s 16.2.8
#define ENS160_TEMP_ADDR 0x13
#define ENS160_HUMIDITY_ADDR 0x15
#define ENS160_OPMODE_RESET 0xF0

// Decodes the 5 contiguous data bytes starting at ENS160_AQI_REG.
ens_160_read_info_t ens160_to_read_info(const uint8_t data[5]);
// Temperature compensation value, Kelvin * 64 (s 16.2.5).
uint16_t ens160_to_temp_value(float temp_celcius);
// Relative humidity compensation value, % * 512 (s 16.2.6).
uint16_t ens160_to_humidity_value(float relative_humidity);
// Fills buffer with {reg_addr, LSB, MSB} for an I2C write.
void ens160_to_temp_buffer(uint8_t temp_reg_addr, uint8_t buffer[3],
						   float temp_celcius);
void ens160_to_humidity_buffer(uint8_t humidity_reg_addr, uint8_t buffer[3],
							   float relative_humidity);

#endif // C_STARTER_ENS160_COMMON_H

//
// Created by eugen on 8/15/2026.
//

#ifndef CPP_STARTER_MODEL_HPP
#define CPP_STARTER_MODEL_HPP
#include <cstdint>

typedef struct ens_160_read_info {
	int8_t agi_uba;
	uint16_t tvoc_ppb;
	uint16_t eco2_ppm;
	uint16_t etoh_ppb;
} ens_160_read_info_t;

typedef struct bmp_280_read_info {
	int16_t temperature_celcius;
	int16_t relative_humidity;
} bmp_280_read_info_t;

#endif // CPP_STARTER_MODEL_HPP
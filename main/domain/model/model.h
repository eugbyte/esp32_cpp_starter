//
// Native C counterpart of model.hpp.
//

#ifndef C_STARTER_MODEL_H
#define C_STARTER_MODEL_H

#include <stdint.h>

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

#endif // C_STARTER_MODEL_H

//
// Native C counterpart of bmp280_service.hpp.
//

#ifndef C_STARTER_BMP280_SERVICE_H
#define C_STARTER_BMP280_SERVICE_H

#include "domain/interface/ibmp280_service.h"
#include "domain/interface/ii2c_service.h"
#include "domain/model/model.h"

#include <stdbool.h>

#define BMP280_REG_CALIB 0x88
#define BMP280_ADDR 0x76
#define BMP280_TEMP_REGISTER_ADDR 0xFA
#define BMP280_PRESSURE_REGISTER_ADDR 0xF7
#define BMP280_REG_ID 0xD0
#define BMP280_REG_CTRL_MEAS 0xF4

typedef struct bmp280_calib {
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
} bmp280_calib_t;

// BMP280 temperature/pressure sensor on an I2C bus. Implements
// ibmp280_service_t; `base` must stay the first member.
typedef struct bmp280_service {
	ibmp280_service_t base;
	ii2c_service_t *i2c_svc;
	i2c_master_dev_handle_t device_handle;
	bmp280_calib_t calib;
	bool subscribed;
} bmp280_service_t;

// Binds the sensor to an I2C bus service. Does not touch the hardware.
void bmp280_service_init(bmp280_service_t *self, ii2c_service_t *i2c_svc);
// Unsubscribes the I2C device and clears the cached calibration values.
void bmp280_service_deinit(bmp280_service_t *self);
// Subscribes the BMP280 on the I2C bus and loads its factory calibration
// values. Must be called before reading.
esp_err_t bmp280_service_connect(bmp280_service_t *self);
// Reads compensated temperature (degC) and pressure (hPa) into *out_info.
// Note: as in the C++ service, pressure is stored in `relative_humidity`.
esp_err_t bmp280_service_read_thermal(bmp280_service_t *self,
									  bmp_280_read_info_t *out_info);
// Reads compensated temperature (degC) and pressure (hPa) as floats.
esp_err_t bmp280_service_read_data(bmp280_service_t *self, float *temperature,
								   float *pressure);

// Upcast to the interface.
static inline ibmp280_service_t *
bmp280_service_as_interface(bmp280_service_t *self) {
	return &self->base;
}

#endif // C_STARTER_BMP280_SERVICE_H

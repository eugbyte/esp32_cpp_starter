//
// Native C counterpart of ens160_i2c_service.hpp.
//

#ifndef C_STARTER_ENS160_I2C_SERVICE_H
#define C_STARTER_ENS160_I2C_SERVICE_H

#include "domain/model/model.h"
#include "service/i2c/i2c_service.h"
#include "ens160_common.h"

#include <stdbool.h>

// ENS160 air quality sensor on an I2C bus.
typedef struct ens160_i2c_service {
	i2c_service_t *i2c_svc;
	i2c_master_dev_handle_t device_handle;
	bool subscribed;
} ens160_i2c_service_t;

// Binds the sensor to an I2C bus service. Does not touch the hardware.
void ens160_i2c_service_init(ens160_i2c_service_t *self,
							 i2c_service_t *i2c_svc);
// Unsubscribes the I2C device.
void ens160_i2c_service_deinit(ens160_i2c_service_t *self);
// Subscribes the sensor, switches it to standard mode and writes the
// optional temperature/humidity compensation values (NULL to skip).
esp_err_t ens160_i2c_service_connect(ens160_i2c_service_t *self,
									 const float *ambient_temp_celcius_opt,
									 const float *ambient_relative_humidity_opt);
// Reads AQI/TVOC/eCO2 into *out_info.
esp_err_t ens160_i2c_service_read_air_data(ens160_i2c_service_t *self,
										   ens_160_read_info_t *out_info);

#endif // C_STARTER_ENS160_I2C_SERVICE_H

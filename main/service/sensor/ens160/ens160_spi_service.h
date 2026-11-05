//
// Native C counterpart of ens160_spi_service.hpp.
//

#ifndef C_STARTER_ENS160_SPI_SERVICE_H
#define C_STARTER_ENS160_SPI_SERVICE_H

#include "domain/interface/iens160_service.h"
#include "domain/interface/ispi_service.h"
#include "ens160_common.h"

#include <stdbool.h>

#define ENS160_READ_BIT 0x01 // s 14.2.3
#define ENS160_WRITE_BIT 0x00
#ifndef PIN_NUM_CS
#define PIN_NUM_CS 5
#endif

// ENS160 air quality sensor on an SPI bus. Implements iens160_service_t;
// `base` must stay the first member.
typedef struct ens160_spi_service {
	iens160_service_t base;
	ispi_service_t *spi_svc;
	spi_device_handle_t device_handle;
	bool subscribed;
} ens160_spi_service_t;

// Binds the sensor to an SPI bus service. Does not touch the hardware.
void ens160_spi_service_init(ens160_spi_service_t *self,
							 ispi_service_t *spi_svc);
// Detaches the SPI device.
void ens160_spi_service_deinit(ens160_spi_service_t *self);
// Attaches the sensor, switches it to standard mode and writes the
// optional temperature/humidity compensation values (NULL to skip).
esp_err_t ens160_spi_service_connect(ens160_spi_service_t *self,
									 const float *ambient_temp_celcius_opt,
									 const float *ambient_relative_humidity_opt);
// Reads AQI/TVOC/eCO2 into *out_info.
esp_err_t ens160_spi_service_read_air_data(ens160_spi_service_t *self,
										   ens_160_read_info_t *out_info);

// Upcast to the interface.
static inline iens160_service_t *
ens160_spi_service_as_interface(ens160_spi_service_t *self) {
	return &self->base;
}

#endif // C_STARTER_ENS160_SPI_SERVICE_H

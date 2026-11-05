//
// Native C counterpart of ispi_service.hpp.
//
// See ii2c_service.h for how C interfaces (vtable + embedded base) work.
//

#ifndef C_STARTER_ISPI_SERVICE_H
#define C_STARTER_ISPI_SERVICE_H

#include <driver/spi_master.h>
#include <esp_err.h>
#include <stddef.h>
#include <stdint.h>

typedef struct ispi_service ispi_service_t;

typedef struct ispi_service_vtable {
	spi_device_interface_config_t (*create_default_device_config)(
		ispi_service_t *self, uint8_t pin_gpio);
	esp_err_t (*subscribe)(ispi_service_t *self, spi_device_handle_t *spi,
						   spi_device_interface_config_t devcfg);
	esp_err_t (*unsubscribe)(ispi_service_t *self, spi_device_handle_t spi);
	esp_err_t (*read_write_byte)(const ispi_service_t *self,
								 spi_device_handle_t spi, uint8_t *rx_data,
								 const uint8_t *tx_data, size_t byte_size);
	esp_err_t (*write_byte)(const ispi_service_t *self,
							spi_device_handle_t spi, const uint8_t *tx_data,
							size_t byte_size);
	// Virtual destructor.
	void (*deinit)(ispi_service_t *self);
} ispi_service_vtable_t;

struct ispi_service {
	const ispi_service_vtable_t *vtable;
};

static inline spi_device_interface_config_t
ispi_service_create_default_device_config(ispi_service_t *self,
										  uint8_t pin_gpio) {
	return self->vtable->create_default_device_config(self, pin_gpio);
}

static inline esp_err_t
ispi_service_subscribe(ispi_service_t *self, spi_device_handle_t *spi,
					   spi_device_interface_config_t devcfg) {
	return self->vtable->subscribe(self, spi, devcfg);
}

static inline esp_err_t ispi_service_unsubscribe(ispi_service_t *self,
												 spi_device_handle_t spi) {
	return self->vtable->unsubscribe(self, spi);
}

static inline esp_err_t
ispi_service_read_write_byte(const ispi_service_t *self,
							 spi_device_handle_t spi, uint8_t *rx_data,
							 const uint8_t *tx_data, size_t byte_size) {
	return self->vtable->read_write_byte(self, spi, rx_data, tx_data,
										 byte_size);
}

static inline esp_err_t ispi_service_write_byte(const ispi_service_t *self,
												spi_device_handle_t spi,
												const uint8_t *tx_data,
												size_t byte_size) {
	return self->vtable->write_byte(self, spi, tx_data, byte_size);
}

static inline void ispi_service_deinit(ispi_service_t *self) {
	self->vtable->deinit(self);
}

#endif // C_STARTER_ISPI_SERVICE_H

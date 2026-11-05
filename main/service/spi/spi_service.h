//
// Native C counterpart of spi.hpp.
//

#ifndef C_STARTER_SPI_SERVICE_H
#define C_STARTER_SPI_SERVICE_H

#include "domain/interface/ispi_service.h"

#include <driver/spi_master.h>

#define GPIO_MISO 19
#define GPIO_MOSI 23
#define GPIO_SCLK 18
#define PIN_NUM_CS 5

/**
 * Full-duplex SPI master service. Implements ispi_service_t; `base` must
 * stay the first member so &self->base can be used as the interface.
 *
 * spi_service_init() initializes the bus (MOSI/MISO/SCLK per the macros
 * above, DMA channel auto-selected); spi_service_deinit() frees it.
 */
typedef struct spi_service {
	ispi_service_t base;
	spi_host_device_t host;
} spi_service_t;

// Initializes the SPI2_HOST bus.
esp_err_t spi_service_init(spi_service_t *self);
// Frees the SPI bus.
void spi_service_deinit(spi_service_t *self);

/**
 * Build a baseline device config: SPI mode 0, 10 MHz clock, queue size 1,
 * CS on `pin_gpio`. Adjust the returned struct before passing it to
 * spi_service_subscribe() if the device needs different timing.
 */
spi_device_interface_config_t
spi_service_create_default_device_config(spi_service_t *self, uint8_t pin_gpio);

// Attach a device to the bus. On success `*spi` receives the handle.
esp_err_t spi_service_subscribe(spi_service_t *self, spi_device_handle_t *spi,
								spi_device_interface_config_t devcfg);

// Detach a device previously added with spi_service_subscribe().
esp_err_t spi_service_unsubscribe(spi_service_t *self, spi_device_handle_t spi);

/**
 * One blocking full-duplex transaction of `byte_size` bytes. `rx_data` and
 * `tx_data` must each be `byte_size` bytes long, and `byte_size` covers the
 * whole frame including register-address byte(s). `rx_data` may be NULL to
 * discard the received bytes.
 */
esp_err_t spi_service_read_write_byte(const spi_service_t *self,
									  spi_device_handle_t spi,
									  uint8_t *rx_data, const uint8_t *tx_data,
									  size_t byte_size);

// Blocking write-only transaction; MISO is discarded.
esp_err_t spi_service_write_byte(const spi_service_t *self,
								 spi_device_handle_t spi,
								 const uint8_t *tx_data, size_t byte_size);

// Upcast to the interface.
static inline ispi_service_t *spi_service_as_interface(spi_service_t *self) {
	return &self->base;
}

#endif // C_STARTER_SPI_SERVICE_H

//
// Native C counterpart of spi.cpp.
//

#include "spi_service.h"

#include <driver/gpio.h>

esp_err_t spi_service_init(spi_service_t *self) {
	self->host = SPI2_HOST;

	spi_bus_config_t buscfg = {
		.mosi_io_num = GPIO_MOSI,
		.miso_io_num = GPIO_MISO,
		.sclk_io_num = GPIO_SCLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
	};

	return spi_bus_initialize(self->host, &buscfg, SPI_DMA_CH_AUTO);
}

void spi_service_deinit(spi_service_t *self) { spi_bus_free(self->host); }

spi_device_interface_config_t
spi_service_create_default_device_config(spi_service_t *self,
										 uint8_t pin_gpio) {
	(void)self;
	spi_device_interface_config_t devcfg = {
		.mode = 0,							// SPI mode 0
		.clock_speed_hz = 10 * 1000 * 1000, // 10 MHz
		.spics_io_num = pin_gpio,
		.queue_size = 1,
	};
	return devcfg;
}

esp_err_t spi_service_subscribe(spi_service_t *self, spi_device_handle_t *spi,
								spi_device_interface_config_t devcfg) {
	return spi_bus_add_device(self->host, &devcfg, spi);
}

esp_err_t spi_service_unsubscribe(spi_service_t *self,
								  spi_device_handle_t spi) {
	(void)self;
	return spi_bus_remove_device(spi);
}

esp_err_t spi_service_read_write_byte(const spi_service_t *self,
									  spi_device_handle_t spi,
									  uint8_t *rx_data, const uint8_t *tx_data,
									  size_t byte_size) {
	(void)self;
	spi_transaction_t t = {
		.length = byte_size * 8, // length is in bits
		.tx_buffer = tx_data,
		.rx_buffer = rx_data,
	};
	return spi_device_transmit(spi, &t); // blocking transmit
}

esp_err_t spi_service_write_byte(const spi_service_t *self,
								 spi_device_handle_t spi,
								 const uint8_t *tx_data, size_t byte_size) {
	return spi_service_read_write_byte(self, spi, NULL, tx_data, byte_size);
}

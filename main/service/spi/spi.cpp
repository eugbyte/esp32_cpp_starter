//
// Created by eugen on 9/1/2026.
//

#include "spi.hpp"
#include <driver/gpio.h>
#include <driver/spi_master.h>

using namespace svc::spi;

static const char *TAG = "spi_example";

SPIService::SPIService() {
	// Configuration for the SPI bus
	spi_bus_config_t buscfg = {.mosi_io_num = GPIO_MOSI,
							   .miso_io_num = GPIO_MISO,
							   .sclk_io_num = GPIO_SCLK,
							   .quadwp_io_num = -1,
							   .quadhd_io_num = -1};

	esp_err_t err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(err);
}

SPIService::~SPIService() { spi_bus_free(SPI2_HOST); }

spi_device_interface_config_t
SPIService::create_default_device_config(uint8_t pin_gpio) {
	spi_device_interface_config_t devcfg = {};
	devcfg.mode = 0; // SPI mode 0
	devcfg.clock_speed_hz = 10 * 1000 * 1000; // 10 MHz
	devcfg.spics_io_num = pin_gpio;
	devcfg.queue_size = 1;
	return devcfg;
}

esp_err_t SPIService::subscribe(spi_device_handle_t spi,
								spi_device_interface_config_t devcfg) {
	// Attach the device to the SPI bus
	esp_err_t err = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
	return err;
}

esp_err_t SPIService::unsubscribe(spi_device_handle_t spi) {
	return spi_bus_remove_device(spi);
}

esp_err_t SPIService::spi_read_write_byte(uint8_t *rx_data,
										  const uint8_t *tx_data,
										  size_t bit_size) const {
	spi_transaction_t t = {};
	t.length = bit_size;
	t.tx_buffer = tx_data;
	t.rx_buffer = rx_data;
	return spi_device_transmit(_spi, &t); // blocking transmit
}
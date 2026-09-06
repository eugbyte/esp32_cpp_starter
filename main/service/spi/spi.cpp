//
// Created by eugen on 9/1/2026.
//

#include "spi.hpp"
#include <driver/gpio.h>
#include <driver/spi_master.h>

using namespace svc::spi;

SPIService::SPIService() {
	// Configuration for the SPI bus
	spi_bus_config_t buscfg = {};
	buscfg.mosi_io_num = GPIO_MOSI;
	buscfg.miso_io_num = GPIO_MISO;
	buscfg.sclk_io_num = GPIO_SCLK;
	buscfg.quadwp_io_num = -1;
	buscfg.quadhd_io_num = -1;

	esp_err_t err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(err);
}

SPIService::~SPIService() { spi_bus_free(SPI2_HOST); }

spi_device_interface_config_t
SPIService::create_default_device_config(uint8_t pin_gpio) {
	spi_device_interface_config_t devcfg = {};
	devcfg.mode = 0;						  // SPI mode 0
	devcfg.clock_speed_hz = 10 * 1000 * 1000; // 10 MHz
	devcfg.spics_io_num = pin_gpio;
	devcfg.queue_size = 1;
	// fixed 8-bit address phase so write_byte/read_byte can set
	// spi_transaction_t.addr directly, with no per-transaction override needed
	devcfg.address_bits = 8;
	return devcfg;
}

esp_err_t SPIService::subscribe(spi_device_handle_t *spi_device,
								spi_device_interface_config_t devcfg) {
	// Attach the device to the SPI bus
	return spi_bus_add_device(SPI2_HOST, &devcfg, spi_device);
}

esp_err_t SPIService::unsubscribe(spi_device_handle_t spi_device) {
	return spi_bus_remove_device(spi_device);
}

esp_err_t SPIService::write_byte(spi_device_handle_t spi_device,
								 uint8_t reg_addr, uint8_t *tx_data,
								 size_t byte_size) {
	// device's address_bits (set to 8 in create_default_device_config) sends
	// reg_addr in its own address phase before tx_data
	spi_transaction_t t = {};
	t.addr = reg_addr;
	t.length = byte_size * 8; // spi_transaction_t.length is in bits
	t.tx_buffer = tx_data;
	t.rx_buffer = nullptr;
	return spi_device_transmit(spi_device, &t);
}

esp_err_t SPIService::read_byte(spi_device_handle_t spi_device,
								uint8_t reg_addr, uint8_t *rx_data,
								size_t byte_size) {
	// device's address_bits (set to 8 in create_default_device_config) sends
	// reg_addr in its own address phase before rx_data
	spi_transaction_t t = {};
	t.addr = reg_addr;
	t.length = byte_size * 8; // spi_transaction_t.length is in bits
	t.rxlength = byte_size * 8;
	t.tx_buffer = nullptr;
	t.rx_buffer = rx_data;
	return spi_device_transmit(spi_device, &t);
}

esp_err_t SPIService::spi_read_write_byte(spi_device_handle_t spi_device,
										  uint8_t *rx_data,
										  const uint8_t *tx_data,
										  size_t byte_size) const {
	spi_transaction_t t = {};
	t.length = byte_size * 8; // spi_transaction_t.length is in bits
	t.tx_buffer = tx_data;
	t.rx_buffer = rx_data;

	return spi_device_transmit(spi_device, &t); // blocking transmit
}
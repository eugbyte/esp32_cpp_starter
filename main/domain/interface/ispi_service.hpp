//
// Created by eugen on 9/2/2026.
//

#ifndef CPP_STARTER_ISPI_SERVICE_HPP
#define CPP_STARTER_ISPI_SERVICE_HPP

#include <driver/spi_master.h>

// Abstraction over the ESP-IDF SPI master driver for a single bus, with one
// device (CS line) attached at a time via subscribe/unsubscribe.
class ISPIService {
public:
	virtual ~ISPIService() = default;

	// Baseline spi_device_interface_config_t for the device on pin_gpio (CS);
	// callers may override individual fields (clock speed, mode, etc.) before
	// passing the result to subscribe().
	virtual spi_device_interface_config_t
	create_default_device_config(uint8_t pin_gpio) = 0;

	// Attaches a device to the bus using devcfg, populating *spi with its handle.
	virtual esp_err_t subscribe(spi_device_handle_t *spi,
								spi_device_interface_config_t devcfg) = 0;
	// Detaches a device previously attached via subscribe().
	virtual esp_err_t unsubscribe(spi_device_handle_t spi) = 0;

	// reg_addr is the device-formatted address byte (e.g. ENS160:
	// (reg << 1) | WRITE_BIT); it is clocked out in its own address phase, so
	// tx_data holds only payload bytes (byte_size excludes reg_addr).
	virtual esp_err_t write_byte(spi_device_handle_t spi_device,
								uint8_t reg_addr, uint8_t *tx_data,
								size_t byte_size) = 0;
	// reg_addr is the device-formatted address byte (e.g. ENS160:
	// (reg << 1) | READ_BIT); it is clocked out in its own address phase, so
	// rx_data holds only payload bytes (byte_size excludes reg_addr).
	virtual esp_err_t read_byte(spi_device_handle_t spi_device,
								uint8_t reg_addr, uint8_t *rx_data,
								size_t byte_size) = 0;

	// Raw full-duplex transaction: tx_data and rx_data are clocked
	// simultaneously byte_size bytes at a time with no dedicated address
	// phase, so a register address (if any) must be embedded as the first
	// byte(s) of tx_data, and the corresponding byte(s) of rx_data will hold
	// whatever garbage MISO returns while that address is being sent.
	virtual esp_err_t spi_read_write_byte(spi_device_handle_t spi,
										  uint8_t *rx_data,
										  const uint8_t *tx_data,
										  size_t byte_size) const = 0;
};

#endif // CPP_STARTER_ISPI_SERVICE_HPP
//
// Created by eugen on 9/2/2026.
//

#ifndef CPP_STARTER_ISPI_SERVICE_HPP
#define CPP_STARTER_ISPI_SERVICE_HPP

#include <driver/spi_master.h>

class ISPIService {
public:
	virtual ~ISPIService() = default;

	virtual spi_device_interface_config_t
	create_default_device_config(uint8_t pin_gpio) = 0;

	virtual esp_err_t subscribe(spi_device_handle_t *spi,
								spi_device_interface_config_t devcfg) = 0;
	virtual esp_err_t unsubscribe(spi_device_handle_t spi) = 0;

	virtual esp_err_t spi_read_write_byte(spi_device_handle_t spi,
										  uint8_t *rx_data,
										  const uint8_t *tx_data,
										  size_t byte_size) const = 0;
	virtual esp_err_t spi_write_byte(spi_device_handle_t spi,
									 const uint8_t *tx_data,
									 size_t byte_size) const = 0;
};

#endif // CPP_STARTER_ISPI_SERVICE_HPP
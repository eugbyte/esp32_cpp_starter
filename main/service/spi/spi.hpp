//
// Created by eugen on 9/1/2026.
//

#ifndef CPP_STARTER_SPI_HPP
#define CPP_STARTER_SPI_HPP
#include "domain/interface/ispi_service.hpp"
#include <driver/spi_master.h>

#define GPIO_MISO 19
#define GPIO_MOSI 23
#define GPIO_SCLK 18
#define PIN_NUM_CS 5

namespace svc::spi {
class SPIService : public ISPIService {
private:
	spi_device_handle_t _spi;

public:
	SPIService();
	~SPIService() override;
	spi_device_interface_config_t
	create_default_device_config(uint8_t pin_gpio) override;
	esp_err_t subscribe(spi_device_handle_t spi,
						spi_device_interface_config_t devcfg) override;
	esp_err_t unsubscribe(spi_device_handle_t spi) override;
	esp_err_t spi_read_write_byte(uint8_t *rx_data, const uint8_t *tx_data,
								  size_t bit_size) const override;
};
} // namespace svc::spi
#endif // CPP_STARTER_SPI_HPP

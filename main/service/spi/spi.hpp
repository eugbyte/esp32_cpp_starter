//
// Created by eugen on 9/1/2026.
//

#ifndef CPP_STARTER_SPI_HPP
#define CPP_STARTER_SPI_HPP
#include <driver/spi_master.h>

#define GPIO_MISO  19
#define GPIO_MOSI  23
#define GPIO_SCLK   18
#define PIN_NUM_CS    5

namespace svc::spi {
class SPIService {
private:
	spi_device_handle_t _spi;
public:
	SPIService();
	~SPIService();
	esp_err_t subscribe(spi_device_handle_t spi, spi_device_interface_config_t devcfg);
};
}
#endif // CPP_STARTER_SPI_HPP

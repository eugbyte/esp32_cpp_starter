//
// Created by eugen on 9/1/2026.
//

#include "spi.hpp"
#include <driver/spi_master.h>
#include <driver/gpio.h>

using namespace svc::spi;

static const char *TAG = "spi_example";

SPIService::SPIService() {
	// Configuration for the SPI bus
	spi_bus_config_t buscfg = {
		.mosi_io_num = GPIO_MOSI,
		.miso_io_num = GPIO_MISO,
		.sclk_io_num = GPIO_SCLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};

	esp_err_t err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(err);
}

esp_err_t SPIService::subscribe(spi_device_handle_t spi,
								spi_device_interface_config_t devcfg) {
	// Attach the device to the SPI bus
	esp_err_t err = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
	return err;
}
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
/**
 * Full-duplex SPI master service on SPI2_HOST.
 *
 * The constructor initializes the SPI2 bus (MOSI/MISO/SCLK per the macros
 * above, DMA channel auto-selected); the destructor frees the bus. Devices
 * are attached with subscribe() and detached with unsubscribe().
 */
class SPIService : public ISPIService {
public:
	SPIService();
	~SPIService() override;

	/**
	 * Build a baseline device config: SPI mode 0, 10 MHz clock, queue size 1,
	 * CS on `pin_gpio`. Adjust the returned struct (clock speed, mode,
	 * cs_ena_pretrans, ...) before passing it to subscribe() if the device
	 * needs different timing.
	 */
	spi_device_interface_config_t
	create_default_device_config(uint8_t pin_gpio) override;

	/**
	 * Attach a device to the SPI2 bus. On success, `*spi` receives the handle
	 * to use for transfers. Returns the esp_err_t from spi_bus_add_device().
	 */
	esp_err_t subscribe(spi_device_handle_t *spi,
						spi_device_interface_config_t devcfg) override;

	/** Detach a device previously added with subscribe(). */
	esp_err_t unsubscribe(spi_device_handle_t spi) override;

	/**
	 * Perform one blocking full-duplex transaction of `byte_size` bytes:
	 * `tx_data` is shifted out on MOSI while MISO is sampled into `rx_data`.
	 *
	 * Both buffers are used for the same single transaction, so `rx_data` and
	 * `tx_data` must each be equally `byte_size` bytes long. `byte_size`
	 * covers the whole frame including the register-address byte(s) at the
	 * start of `tx_data` — e.g. reading 5 data bytes behind a 1-byte register
	 * address requires byte_size = 6.
	 *
	 * `rx_data` may be nullptr to discard the received bytes (write-only).
	 */
	esp_err_t spi_read_write_byte(spi_device_handle_t spi, uint8_t *rx_data,
								  const uint8_t *tx_data,
								  size_t byte_size) const override;

	/**
	 * Blocking write-only transaction: transmits `byte_size` bytes from
	 * `tx_data` and discards MISO. As with spi_read_write_byte(), `byte_size`
	 * is the full frame length including any register-address byte(s)
	 * contained in `tx_data`.
	 */
	esp_err_t spi_write_byte(spi_device_handle_t spi, const uint8_t *tx_data,
							 size_t byte_size) const override;
};
} // namespace svc::spi
#endif // CPP_STARTER_SPI_HPP

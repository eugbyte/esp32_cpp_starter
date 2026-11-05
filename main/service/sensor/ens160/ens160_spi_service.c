//
// Native C counterpart of ens160_spi_service.cpp.
//

#include "ens160_spi_service.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include <string.h>

static const char TAG[] = "ens160";

// SPI address byte: register address shifted left, R/W flag in bit 0
// (s 14.2.2, s 14.2.3).
#define ENS160_SPI_ADDR(reg, rw) ((uint8_t)(((reg) << 1) | (rw)))

// Switches to standard (continuous) measurement and standard power mode.
static esp_err_t set_normal_mode(ens160_spi_service_t *self) {
	uint8_t tx_data[2] = {
		ENS160_SPI_ADDR(ENS160_OP_MODE_ADDR, ENS160_WRITE_BIT),
		ENS160_NORMAL_MODE,
	};
	esp_err_t err = spi_service_write_byte(self->spi_svc, self->device_handle,
											tx_data, sizeof(tx_data));
	if (err != ESP_OK) {
		return err;
	}
	vTaskDelay(pdMS_TO_TICKS(20));
	return err;
}

// Writes a 16-bit value LSB-first to reg and reg+1. SPI writes do not
// auto-increment the register address (s 14.2.4), so each data byte gets
// its own address byte.
static esp_err_t write_u16(ens160_spi_service_t *self, uint8_t reg,
						   uint16_t value) {
	uint8_t buffer[4] = {
		ENS160_SPI_ADDR(reg, ENS160_WRITE_BIT),
		(uint8_t)(value & 0xFF), // LSB
		ENS160_SPI_ADDR(reg + 1, ENS160_WRITE_BIT),
		(uint8_t)(value >> 8), // MSB
	};
	return spi_service_write_byte(self->spi_svc, self->device_handle, buffer,
								   sizeof(buffer));
}

// Writes temperature and humidity compensation values (s 16.2.5 -
// s 16.2.6). Either pointer may be NULL to skip it.
static esp_err_t set_compensation_values(ens160_spi_service_t *self,
										 const float *temp_celcius_opt,
										 const float *relative_humidity_opt) {
	if (temp_celcius_opt != NULL) {
		write_u16(self, ENS160_TEMP_ADDR,
				  ens160_to_temp_value(*temp_celcius_opt));
	}
	if (relative_humidity_opt != NULL) {
		write_u16(self, ENS160_HUMIDITY_ADDR,
				  ens160_to_humidity_value(*relative_humidity_opt));
	}
	return ESP_OK;
}

void ens160_spi_service_init(ens160_spi_service_t *self,
							 spi_service_t *spi_svc) {
	memset(self, 0, sizeof(*self));
	self->spi_svc = spi_svc;
}

void ens160_spi_service_deinit(ens160_spi_service_t *self) {
	if (!self->subscribed) {
		return;
	}
	spi_service_unsubscribe(self->spi_svc, self->device_handle);
	self->subscribed = false;
}

esp_err_t
ens160_spi_service_connect(ens160_spi_service_t *self,
						   const float *ambient_temp_celcius_opt,
						   const float *ambient_relative_humidity_opt) {
	spi_device_interface_config_t devcfg =
		spi_service_create_default_device_config(self->spi_svc, PIN_NUM_CS);
	// Pins 18/19/23 route through the GPIO matrix on SPI2_HOST, so
	// full-duplex MISO sampling shifts one bit late above ~8 MHz; keep the
	// clock below that
	devcfg.clock_speed_hz = 1 * 1000 * 1000;
	// only blocking transmits are used, so at most one transaction is ever
	// in flight
	devcfg.queue_size = 1;
	// ENS160 requires SPI mode 0: CPOL=0, CPHA=0 (s 14.2.1)
	devcfg.mode = 0;
	// assert CS one bit-cycle before the first clock edge so the sensor
	// meets its CS-setup time and doesn't miss the first address bit
	devcfg.cs_ena_pretrans = 1;

	esp_err_t err =
		spi_service_subscribe(self->spi_svc, &self->device_handle, devcfg);
	if (err != ESP_OK) {
		return err;
	}
	self->subscribed = true;

	uint8_t rx_data[3] = {7, 7, 7};
	uint8_t tx_data[3] = {ENS160_SPI_ADDR(ENS160_REG_ID, ENS160_READ_BIT)};
	err = spi_service_read_write_byte(self->spi_svc, self->device_handle,
									   rx_data, tx_data, sizeof(rx_data));
	if (err != ESP_OK) {
		return err;
	}
	ESP_LOGI(TAG, "WHO_AM_I = %X%X", rx_data[1], rx_data[2]);

	err = set_normal_mode(self);
	if (err != ESP_OK) {
		return err;
	}
	return set_compensation_values(self, ambient_temp_celcius_opt,
								   ambient_relative_humidity_opt);
}

esp_err_t ens160_spi_service_read_air_data(ens160_spi_service_t *self,
										   ens_160_read_info_t *out_info) {
	// 1 address byte + 5 data bytes (AGI, TVOC x2, ECO2 x2),
	// s 16.2.8 - 16.2.10
	uint8_t rx_data[1 + 5] = {0};
	uint8_t tx_data[1 + 5] = {ENS160_SPI_ADDR(ENS160_AQI_REG, ENS160_READ_BIT)};

	esp_err_t err = spi_service_read_write_byte(
		self->spi_svc, self->device_handle, rx_data, tx_data, sizeof(rx_data));
	if (err != ESP_OK) {
		memset(out_info, 0, sizeof(*out_info));
		return err;
	}

	// skip the address-byte slot
	*out_info = ens160_to_read_info(&rx_data[1]);
	return ESP_OK;
}

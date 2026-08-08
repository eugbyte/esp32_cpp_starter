//
// Created by eugen on 8/6/2026.
//

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "i2c.hpp"
#include <esp_log.h>

using namespace svc::i2c;

I2CService::I2CService() {

	// 1. Configure and create the I2C bus (master)
	i2c_master_bus_config_t bus_config = {};
	bus_config.i2c_port = I2C_MASTER_NUM;
	bus_config.sda_io_num = static_cast<gpio_num_t>(I2C_MASTER_SDA_IO);
	bus_config.scl_io_num = static_cast<gpio_num_t>(I2C_MASTER_SCL_IO);
	bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
	bus_config.glitch_ignore_cnt = 7;
	bus_config.flags.enable_internal_pullup = true;

	ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle_));
}

I2CService::~I2CService() { i2c_del_master_bus(bus_handle_); }

esp_err_t I2CService::subscribe(uint8_t sensor_address,
								i2c_master_dev_handle_t *device_handle) const {
	i2c_device_config_t dev_config = {};

	dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
	dev_config.device_address = sensor_address;
	dev_config.scl_speed_hz = I2C_MASTER_FREQ_HZ;
	return i2c_master_bus_add_device(bus_handle_, &dev_config, device_handle);
}

esp_err_t
I2CService::unsubscribe(const i2c_master_dev_handle_t *device_handle) const {
	return i2c_master_bus_rm_device(*device_handle);
}

esp_err_t I2CService::read(i2c_master_dev_handle_t device_handle,
						   const uint8_t reg_addr, uint8_t *data,
						   const size_t byte_size) {
	return i2c_master_transmit_receive(device_handle, &reg_addr, 1, data, byte_size,
									   I2C_MASTER_TIMEOUT_MS);
}

esp_err_t I2CService::write(i2c_master_dev_handle_t device_handle,
							const uint8_t reg_addr, uint8_t data) {
	uint8_t write_buf[2] = {reg_addr, data};
	return i2c_master_transmit(device_handle, write_buf, sizeof(write_buf),
							   I2C_MASTER_TIMEOUT_MS);
}

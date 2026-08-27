//
// Created by eugen on 8/8/2026.
//

#ifndef CPP_STARTER_II2C_SERVICE_HPP
#define CPP_STARTER_II2C_SERVICE_HPP

#include <driver/i2c_master.h>

class II2CService {
public:
	virtual ~II2CService() = default;

	// Registers a device at sensor_address on the I2C bus and returns
	// its handle via device_handle.
	virtual esp_err_t
	subscribe(uint8_t sensor_address,
			  i2c_master_dev_handle_t *device_handle) const = 0;

	// Removes a previously subscribed device from the I2C bus.
	virtual esp_err_t
	unsubscribe(const i2c_master_dev_handle_t *device_handle) const = 0;

	// Reads byte_size bytes starting at reg_addr from the given device
	// into data.
	virtual esp_err_t read(i2c_master_dev_handle_t device_handle,
						   uint8_t reg_addr, uint8_t *data,
						   size_t byte_size) = 0;

	virtual esp_err_t write_buffer(i2c_master_dev_handle_t device_handle,
								   const uint8_t *data_buffer_w_reg_addr_prefix,
								   size_t byte_size) = 0;
};

#endif // CPP_STARTER_II2C_SERVICE_HPP

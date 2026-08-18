//
// Created by eugen on 8/6/2026.
//

#ifndef CPP_STARTER_I2C_HPP
#define CPP_STARTER_I2C_HPP

#include "domain/interface/ii2c_service.hpp"
#include <driver/i2c_master.h>

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_TIMEOUT_MS 1000

namespace svc::i2c {
class I2CService : public II2CService {
private:
	i2c_master_bus_handle_t bus_handle_ = {};

public:
	// Configures and creates the I2C master bus (pins, clock source,
	// glitch filter, internal pull-ups) from the I2C_MASTER_* macros.
	I2CService();
	// Deletes the I2C master bus.
	~I2CService() override;
	// Adds a device at sensor_address to the bus and returns its
	// handle via device_handle.
	esp_err_t subscribe(uint8_t sensor_address,
						i2c_master_dev_handle_t *device_handle) const override;
	// Removes a previously added device from the bus.
	esp_err_t
	unsubscribe(const i2c_master_dev_handle_t *device_handle) const override;
	// Writes reg_addr then reads byte_size bytes back from the device
	// into data (combined write-then-read I2C transaction).
	esp_err_t read(i2c_master_dev_handle_t device_handle, uint8_t reg_addr,
				   uint8_t *data, size_t byte_size) override;
	// Writes multiple data bytes to the device, with the first byte being the
	// register address.
	esp_err_t write_buffer(i2c_master_dev_handle_t device_handle,
						   const uint8_t *data_buffer_w_reg_addr_prefix,
						   size_t byte_size) override;
};
} // namespace svc::i2c

#endif // CPP_STARTER_I2C_HPP

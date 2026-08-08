//
// Created by eugen on 8/6/2026.
//

#ifndef CPP_STARTER_I2C_HPP
#define CPP_STARTER_I2C_HPP

#include <driver/i2c_master.h>
#include "domain/interface/ii2c_service.hpp"

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
	I2CService();
	~I2CService() override;
	esp_err_t subscribe(uint8_t sensor_address,
						i2c_master_dev_handle_t *device_handle) const override;
	esp_err_t unsubscribe(const i2c_master_dev_handle_t *device_handle) const override;
	esp_err_t read(i2c_master_dev_handle_t device_handle, uint8_t reg_addr,
				   uint8_t *data, size_t byte_size) override;
	esp_err_t write(i2c_master_dev_handle_t device_handle, uint8_t reg_addr,
					uint8_t data) override;
};
} // namespace svc::i2c

#endif // CPP_STARTER_I2C_HPP

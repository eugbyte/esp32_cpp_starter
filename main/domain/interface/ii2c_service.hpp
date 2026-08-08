//
// Created by eugen on 8/8/2026.
//

#ifndef CPP_STARTER_II2C_SERVICE_HPP
#define CPP_STARTER_II2C_SERVICE_HPP

#include <driver/i2c_master.h>

class II2CService {
public:
	virtual ~II2CService() = default;
	virtual esp_err_t subscribe(uint8_t sensor_address,
						i2c_master_dev_handle_t *device_handle) const = 0;
	virtual esp_err_t unsubscribe(const i2c_master_dev_handle_t *device_handle) const = 0;
	virtual esp_err_t read(i2c_master_dev_handle_t device_handle, uint8_t reg_addr,
				   uint8_t *data, size_t byte_size) = 0;
	virtual esp_err_t write(i2c_master_dev_handle_t device_handle, uint8_t reg_addr,
					uint8_t data) = 0;
};


#endif // CPP_STARTER_II2C_SERVICE_HPP

//
// Created by eugen on 8/15/2026.
//

#ifndef CPP_STARTER_ENS160_I2C_SERVICE_HPP
#define CPP_STARTER_ENS160_I2C_SERVICE_HPP

#include "domain/interface/iens160_service.hpp"
#include "domain/interface/ii2c_service.hpp"
#include "domain/model/model.hpp"
#include "esp_err.h"

#include <etl/tuple.h>

// The I²C slave address is 0x52 (when MISO/ADDR is set low via ground) or 0x53
// (MISO/ADDR is set high via pull-up). (s 14)
#define ENS160_ADDR 0x52
#define ENS160_REG_ID 0x60		 // s 16.2.1
#define ENS160_OP_MODE_ADDR 0x10 // s 16.2.2
#define ENS160_NORMAL_MODE 0x02	 // s 16.2.2
#define ENS160_AQI_REG 0x21		 // s 16.2.8
#define ENS160_TEMP_ADDR 0x13
#define ENS160_HUMIDITY_ADDR 0x15

namespace svc::sensor {
class Ens160Service_I2C : public IEns160Service {
public:
	Ens160Service_I2C(II2CService &i2c_service);
	~Ens160Service_I2C();
	esp_err_t connect(const float *ambient_temp_celcius_opt,
					  const float *ambient_relative_humidity_opt);
	etl::tuple<ens_160_read_info_t, esp_err_t> read_air_data();

private:
	i2c_master_dev_handle_t ens160_device_handle_ = {};
	II2CService &i2c_svc_;
	esp_err_t set_normal_mode() const;
	esp_err_t ens160_read_data(int8_t *agi, int16_t *tvoc, int16_t *eco2,
							   int16_t *etoh);
	esp_err_t set_compensation_values(const float *temp_celcius_opt,
									  const float *relative_humidity_opt) const;
};
} // namespace svc::sensor

#endif // CPP_STARTER_ENS160_I2C_SERVICE_HPP

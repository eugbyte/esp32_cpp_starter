//
// Created by eugen on 8/15/2026.
//

#ifndef CPP_STARTER_ENS160_I2C_SERVICE_HPP
#define CPP_STARTER_ENS160_I2C_SERVICE_HPP

#include "domain/interface/iens160_service.hpp"
#include "domain/interface/ii2c_service.hpp"
#include "domain/model/model.hpp"
#include "ens160_common.hpp"
#include "esp_err.h"

#include <etl/tuple.h>

namespace svc::sensor::ens160 {
class Ens160Service_I2C : public IEns160Service {
public:
	Ens160Service_I2C(II2CService &i2c_service);
	~Ens160Service_I2C() override;
	esp_err_t connect(const float *ambient_temp_celcius_opt,
					  const float *ambient_relative_humidity_opt) override;
	etl::tuple<ens_160_read_info_t, esp_err_t> read_air_data() override;

private:
	i2c_master_dev_handle_t ens160_device_handle_ = {};
	II2CService &i2c_svc_;
	esp_err_t set_normal_mode() const;
	esp_err_t set_compensation_values(const float *temp_celcius_opt,
									  const float *relative_humidity_opt) const;
};
} // namespace svc::sensor::ens160

#endif // CPP_STARTER_ENS160_I2C_SERVICE_HPP

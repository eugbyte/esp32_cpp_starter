//
// Created by eugen on 9/3/2026.
//

#ifndef CPP_STARTER_IENS160_SERVICE_HPP
#define CPP_STARTER_IENS160_SERVICE_HPP

#include "domain/model/model.hpp"

#include <esp_err.h>
#include <etl/tuple.h>

namespace svc::sensor {

class IEns160Service {
public:
	virtual ~IEns160Service() = default;

	virtual esp_err_t connect(const float *ambient_temp_celcius_opt,
							 const float *ambient_relative_humidity_opt) = 0;
	virtual etl::tuple<ens_160_read_info_t, esp_err_t> read_air_data() = 0;
};

} // namespace svc::sensor

#endif // CPP_STARTER_IENS160_SERVICE_HPP
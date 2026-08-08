//
// Created by eugen on 8/8/2026.
//

#ifndef CPP_STARTER_BMP280_SERVICE_HPP
#define CPP_STARTER_BMP280_SERVICE_HPP
#include "domain/interface/ii2c_service.hpp"

#include <etl/tuple.h>

namespace svc::bmp280_sensor {

class Bmp280Service {
public:
	Bmp280Service(II2CService& i2c_service);
	etl::tuple<float, esp_err_t> Bmp280Service::bmp280_read_temp();
private:
	i2c_master_dev_handle_t bmp280_device_handle_ = {};
	II2CService& i2c_svc_;
};

void bmp280_task();
} // namespace svc::bmp280_sensor

#endif // CPP_STARTER_BMP280_SERVICE_HPP

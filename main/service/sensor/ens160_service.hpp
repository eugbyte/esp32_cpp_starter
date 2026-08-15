//
// Created by eugen on 8/15/2026.
//

#ifndef CPP_STARTER_ENS160_SERVICE_HPP
#define CPP_STARTER_ENS160_SERVICE_HPP

#include "domain/interface/ii2c_service.hpp"
#include "esp_err.h"

// The I²C slave address is 0x52 (when MISO/ADDR is set low via ground) or 0x53 (MISO/ADDR is set high via pull-up).
#define ENS160_ADDR 0x52
#define ENS160_REG_ID 0x60

namespace svc::sensor {
class Ens160Service {
public:
	Ens160Service(II2CService &i2c_service);
	esp_err_t subscribe();
private:
	i2c_master_dev_handle_t ens160_device_handle_ = {};
	II2CService &i2c_svc_;
};
}


#endif // CPP_STARTER_ENS160_SERVICE_HPP

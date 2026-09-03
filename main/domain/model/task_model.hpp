//
// Created by eugen on 8/16/2026.
//

#ifndef CPP_STARTER_TASK_MODEL_HPP
#define CPP_STARTER_TASK_MODEL_HPP

#include "domain/interface/ibmp280_service.hpp"
#include "domain/interface/iens160_service.hpp"

typedef struct {
	svc::sensor::IBmp280Service *bmp280_service;
	svc::sensor::IEns160Service *ens160_service;
} i2c_sensor_services_t;

#endif // CPP_STARTER_TASK_MODEL_HPP

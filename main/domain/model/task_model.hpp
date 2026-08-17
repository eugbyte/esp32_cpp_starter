//
// Created by eugen on 8/16/2026.
//

#ifndef CPP_STARTER_TASK_MODEL_HPP
#define CPP_STARTER_TASK_MODEL_HPP

#include "service/sensor/ens160_service.hpp"
#include "service/sensor/bmp280_service.hpp"

typedef struct {
	svc::sensor::Bmp280Service *bmp280_service;
	svc::sensor::Ens160Service *ens160_service;
} i2c_sensor_services_t;

#endif // CPP_STARTER_TASK_MODEL_HPP

//
// Created by eugen on 8/9/2026.
//

#ifndef CPP_STARTER_I2C_SENSOR_TASK_HPP
#define CPP_STARTER_I2C_SENSOR_TASK_HPP
#include "service/bmp280_sensor/bmp280_service.hpp"

#include <esp_err.h>

#define I2C_SENSOR_STACK_SIZE 4096
#define I2C_SENSOR_TASK_PRIORITY 1
#define I2C_SENSOR_CORE_ID 1

namespace task {
// Connects to the BMP280 and spawns a pinned FreeRTOS task that polls
// its temperature/pressure once a second for the lifetime of the app.
esp_err_t i2c_sensor_task(svc::sensor::Bmp280Service &bmp280_service);
}

#endif // CPP_STARTER_I2C_SENSOR_TASK_HPP

//
// Created by eugen on 9/5/2026.
//

#ifndef CPP_STARTER_SPI_SENSOR_TASK_HPP
#define CPP_STARTER_SPI_SENSOR_TASK_HPP
#include "domain/interface/iens160_service.hpp"

#include <esp_err.h>

#define SPI_SENSOR_STACK_SIZE 4096
#define SPI_SENSOR_TASK_PRIORITY 1
#define SPI_SENSOR_CORE_ID 1

namespace task {
// Connects to the ENS160 over SPI and spawns a pinned FreeRTOS task that
// polls its air-quality data every 5 seconds for the lifetime of the app.
esp_err_t spi_sensor_task(IEns160Service &ens160_service);
} // namespace task

#endif // CPP_STARTER_SPI_SENSOR_TASK_HPP

//
// Native C counterpart of i2c_sensor_task.hpp. Also replaces the C++
// i2c_sensor_services_t from task_model.hpp: the sensors are members here.
//

#ifndef C_STARTER_I2C_SENSOR_TASK_H
#define C_STARTER_I2C_SENSOR_TASK_H

#include "domain/interface/ibmp280_service.h"
#include "domain/interface/iens160_service.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_err.h>

#define I2C_SENSOR_STACK_SIZE 4096
#define I2C_SENSOR_TASK_PRIORITY 1
#define I2C_SENSOR_CORE_ID 1
#define I2C_SENSOR_POLL_MS 5000

// Polls the BMP280 and ENS160 on a pinned FreeRTOS task. The struct is the
// task's argument, so it must outlive the task (give it static storage).
typedef struct i2c_sensor_task {
	ibmp280_service_t *bmp280_service;
	iens160_service_t *ens160_service;
	TaskHandle_t task_handle;
} i2c_sensor_task_t;

void i2c_sensor_task_init(i2c_sensor_task_t *self,
						  ibmp280_service_t *bmp280_service,
						  iens160_service_t *ens160_service);
// Stops the polling task if it was started.
void i2c_sensor_task_deinit(i2c_sensor_task_t *self);
// Connects the BMP280, uses its temperature to compensate the ENS160, then
// spawns the polling task (every I2C_SENSOR_POLL_MS).
esp_err_t i2c_sensor_task_start(i2c_sensor_task_t *self);

#endif // C_STARTER_I2C_SENSOR_TASK_H

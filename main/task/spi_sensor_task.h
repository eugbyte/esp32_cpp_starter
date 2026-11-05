//
// Native C counterpart of spi_sensor_task.hpp.
//

#ifndef C_STARTER_SPI_SENSOR_TASK_H
#define C_STARTER_SPI_SENSOR_TASK_H

#include "service/sensor/ens160/ens160_spi_service.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_err.h>

#define SPI_SENSOR_STACK_SIZE 4096
#define SPI_SENSOR_TASK_PRIORITY 1
#define SPI_SENSOR_CORE_ID 1
#define SPI_SENSOR_POLL_MS 5000

// Polls the ENS160 over SPI on a pinned FreeRTOS task. The struct is the
// task's argument, so it must outlive the task (give it static storage).
typedef struct spi_sensor_task {
	ens160_spi_service_t *ens160_service;
	TaskHandle_t task_handle;
} spi_sensor_task_t;

void spi_sensor_task_init(spi_sensor_task_t *self,
						  ens160_spi_service_t *ens160_service);
// Stops the polling task if it was started.
void spi_sensor_task_deinit(spi_sensor_task_t *self);
// Connects the ENS160 (no compensation: no ambient sensor on the SPI bus)
// and spawns the polling task (every SPI_SENSOR_POLL_MS).
esp_err_t spi_sensor_task_start(spi_sensor_task_t *self);

#endif // C_STARTER_SPI_SENSOR_TASK_H

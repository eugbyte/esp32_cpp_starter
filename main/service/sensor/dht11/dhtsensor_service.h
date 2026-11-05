//
// Native C counterpart of dhtsensor_service.hpp.
//

#ifndef C_STARTER_DHTSENSOR_SERVICE_H
#define C_STARTER_DHTSENSOR_SERVICE_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_err.h>

#define DHT11_TASK_STACK_SIZE 4096
#define DHT11_TASK_PRIORITY 5

#define DHT11_TASK_CORE_ID 1 // assign dht to its own core
#define SENSOR_TYPE DHT_TYPE_DHT11

#define DHT_GPIO 25

// DHT11 sensor polled on its own FreeRTOS task. The instance is passed to
// that task, so it must outlive it (give it static storage).
typedef struct dhtsensor_service {
	volatile float temperature;
	volatile float humidity;
	TaskHandle_t task_handle;
} dhtsensor_service_t;

void dhtsensor_service_init(dhtsensor_service_t *self);
// Stops the polling task if it was started.
void dhtsensor_service_deinit(dhtsensor_service_t *self);
// Spawns the polling task pinned to DHT11_TASK_CORE_ID.
esp_err_t dhtsensor_service_start(dhtsensor_service_t *self);
// Most recent temperature; 0 until the first successful read.
float dhtsensor_service_get_temperature(const dhtsensor_service_t *self);
// Most recent humidity; 0 until the first successful read.
float dhtsensor_service_get_humidity(const dhtsensor_service_t *self);

#endif // C_STARTER_DHTSENSOR_SERVICE_H

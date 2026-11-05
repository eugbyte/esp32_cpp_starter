//
// Native C counterpart of dhtsensor_service.cpp.
//

#include "dhtsensor_service.h"

#include <dht.h>
#include <driver/gpio.h>
#include <esp_log.h>

#include <stdbool.h>

static const char TAG[] = "dhtsensor_service";

// Task loop: enables the GPIO pull-up required by the DHT one-wire
// protocol, then polls the sensor every 2 seconds. Runs forever.
static void dht_task(void *parameter) {
	dhtsensor_service_t *self = (dhtsensor_service_t *)parameter;
	gpio_pullup_en((gpio_num_t)DHT_GPIO);

	while (true) {
		float humidity = 0;
		float temperature = 0;
		esp_err_t err = dht_read_float_data(SENSOR_TYPE, (gpio_num_t)DHT_GPIO,
											&humidity, &temperature);
		if (err == ESP_OK) {
			self->humidity = humidity;
			self->temperature = temperature;
		} else {
			ESP_LOGW(TAG, "Could not read data from sensor");
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void dhtsensor_service_init(dhtsensor_service_t *self) {
	self->temperature = 0;
	self->humidity = 0;
	self->task_handle = NULL;
}

void dhtsensor_service_deinit(dhtsensor_service_t *self) {
	if (self->task_handle != NULL) {
		vTaskDelete(self->task_handle);
		self->task_handle = NULL;
	}
}

esp_err_t dhtsensor_service_start(dhtsensor_service_t *self) {
	BaseType_t ok = xTaskCreatePinnedToCore(
		dht_task, "DHT11_task", DHT11_TASK_STACK_SIZE, self,
		DHT11_TASK_PRIORITY, &self->task_handle, DHT11_TASK_CORE_ID);
	return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

float dhtsensor_service_get_temperature(const dhtsensor_service_t *self) {
	return self->temperature;
}

float dhtsensor_service_get_humidity(const dhtsensor_service_t *self) {
	return self->humidity;
}

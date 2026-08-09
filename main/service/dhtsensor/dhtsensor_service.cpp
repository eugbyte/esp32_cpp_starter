//
// Created by eugen on 7/11/2026.
//

#include "freertos/idf_additions.h"

#include "dht.h"
#include "dhtsensor_service.hpp"

#include <driver/gpio.h>

DhtSensorService::DhtSensorService() = default;

DhtSensorService::~DhtSensorService() = default;

// Returns the most recently read temperature (updated by dht_task on
// its own FreeRTOS task); 0 until the first successful read.
float DhtSensorService::get_temperature() { return temperature_; }

// Returns the most recently read humidity (updated by dht_task on its
// own FreeRTOS task); 0 until the first successful read.
float DhtSensorService::get_humidity() { return humidity_; }

// Background task loop: enables the GPIO pull-up required by the DHT
// one-wire protocol, then polls the sensor every 2 seconds, updating
// temperature_/humidity_ in place. Runs forever once started.
void DhtSensorService::dht_task() {
	gpio_pullup_en(static_cast<gpio_num_t>(DHT_GPIO));

	while (true) {
		esp_err_t err =
			dht_read_float_data(SENSOR_TYPE, static_cast<gpio_num_t>(DHT_GPIO),
								&humidity_, &temperature_);
		if (err != ESP_OK) {
			printf("Could not read data from sensor\n");
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

// Spawns dht_task on its own pinned FreeRTOS task/core so sensor
// polling doesn't block the caller.
void DhtSensorService::start() {
	auto dht_task_ = [](void *parameter) {
		auto *service = static_cast<DhtSensorService *>(parameter);
		service->dht_task();
	};
	xTaskCreatePinnedToCore(dht_task_, "DHT11_task", DHT11_TASK_STACK_SIZE,
							NULL, DHT11_TASK_PRIORITY, NULL, APP_CPU_NUM);
}

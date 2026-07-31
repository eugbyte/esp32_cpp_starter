//
// Created by eugen on 7/11/2026.
//

#include "freertos/idf_additions.h"

#include "dht.h"
#include "dhtsensor_service.hpp"

#include "dhtsensor_service.hpp"

#include <driver/gpio.h>

class DhtSensorService {
private:
	float temperature_ = 0;
	float humidity_ = 0;
	void dht_task();

public:
	DhtSensorService();
	~DhtSensorService();
	float get_temperature();
	float get_humidity();
	void start();
};

DhtSensorService::DhtSensorService() = default;

DhtSensorService::~DhtSensorService() = default;

float DhtSensorService::get_temperature() { return temperature_; }

float DhtSensorService::get_humidity() { return humidity_; }

void DhtSensorService::dht_task() {
	gpio_pullup_en(static_cast<gpio_num_t>(DHT_GPIO));

	while (true) {
		esp_err_t err = dht_read_float_data(SENSOR_TYPE, static_cast<gpio_num_t>(DHT_GPIO),
								&humidity_, &temperature_);
		if (err != ESP_OK) {
			printf("Could not read data from sensor\n");
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void DhtSensorService::start() {
	xTaskCreatePinnedToCore(dht_task, "DHT11_task", DHT11_TASK_STACK_SIZE, NULL,
							DHT11_TASK_PRIORITY, NULL, APP_CPU_NUM);
}

//
// Created by eugen on 7/11/2026.
//

#ifndef CPP_STARTER_DHTSENSOR_SERVICE_HPP
#define CPP_STARTER_DHTSENSOR_SERVICE_HPP

#define DHT11_TASK_STACK_SIZE 4096
#define DHT11_TASK_PRIORITY 5

#define DHT11_TASK_CORE_ID 1 // assign dht to its own core
#define SENSOR_TYPE DHT_TYPE_DHT11

#define DHT_GPIO 25

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

#endif // CPP_STARTER_DHTSENSOR_SERVICE_HPP

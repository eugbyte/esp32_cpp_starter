//
// Created by eugen on 8/15/2026.
//

#include "ens160_service.hpp"

#include <esp_log.h>

using namespace svc::sensor;

Ens160Service::Ens160Service(II2CService &i2c_service) :
	i2c_svc_(i2c_service) {}

esp_err_t Ens160Service::subscribe() {
	uint8_t data[2] = {};
	esp_err_t err = i2c_svc_.subscribe(ENS160_ADDR, &ens160_device_handle_);
	if (err != ESP_OK) {
		return err;
	}
	i2c_svc_.read(ens160_device_handle_, ENS160_REG_ID, data, 1);
	ESP_LOGI("i2c", "WHO_AM_I = %X", data[0]);
	return ESP_OK;
}
//
// Created by eugen on 8/8/2026.
//

#ifndef CPP_STARTER_BMP280_SERVICE_HPP
#define CPP_STARTER_BMP280_SERVICE_HPP
#include "domain/interface/ii2c_service.hpp"

#include <etl/tuple.h>

#define BMP280_REG_CALIB 0x88
#define BMP280_ADDR 0x76
#define BMP280_TEMP_REGISTER_ADDR 0xFA
#define BMP280_PRESSURE_REGISTER_ADDR 0xF7
#define BMP280_REG_ID 0xD0

namespace svc::sensor {

typedef struct {
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
} bmp280_calib_t;

class Bmp280Service {
public:
	Bmp280Service(II2CService &i2c_service);
	~Bmp280Service();
	esp_err_t connect();
	etl::tuple<float, esp_err_t> bmp280_read_temp();
	etl::tuple<float, esp_err_t> bmp280_read_pressure();

private:
	i2c_master_dev_handle_t bmp280_device_handle_ = {};
	II2CService &i2c_svc_;
	bmp280_calib_t bmp280_calib_ = {};
	esp_err_t subscribe();
	esp_err_t init_compensation_values();
	esp_err_t bmp280_read_data(float *temperature, float *pressure);
	float bmp280_compensate_temp(int32_t adc_T, int32_t *fine_temp) const;
	float bmp280_compensate_pressure(int32_t adc_P, int32_t fine_temp) const;
};

void bmp280_task();
} // namespace svc::sensor

#endif // CPP_STARTER_BMP280_SERVICE_HPP

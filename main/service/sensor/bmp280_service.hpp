//
// Created by eugen on 8/8/2026.
//

#ifndef CPP_STARTER_BMP280_SERVICE_HPP
#define CPP_STARTER_BMP280_SERVICE_HPP
#include "domain/interface/ii2c_service.hpp"
#include "domain/model/model.hpp"

#include <etl/tuple.h>

#define BMP280_REG_CALIB 0x88
#define BMP280_ADDR 0x76
#define BMP280_TEMP_REGISTER_ADDR 0xFA
#define BMP280_PRESSURE_REGISTER_ADDR 0xF7
#define BMP280_REG_ID 0xD0
#define BMP280_REG_CTRL_MEAS 0xF4

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
	// Unsubscribes the I2C device handle and resets the cached
	// compensation values.
	~Bmp280Service();
	// Subscribes the BMP280 on the I2C bus and loads its factory
	// calibration/compensation values. Must be called before reading.
	esp_err_t connect();
	// Reads and returns compensated temperature (°C) and pressure (hPa).
	etl::tuple<bmp_280_read_info_t, esp_err_t> read_thermal();

private:
	i2c_master_dev_handle_t bmp280_device_handle_ = {};
	II2CService &i2c_svc_;
	bmp280_calib_t bmp280_calib_ = {};
	// Registers the BMP280 device on the I2C bus and logs its WHO_AM_I
	// register value.
	esp_err_t subscribe();
	// Writes ctrl_meas to put the sensor into normal mode with x1
	// oversampling so it actually performs conversions.
	esp_err_t set_normal_sampling_mode() const;
	// Reads the factory calibration registers from the sensor into
	// bmp280_calib_.
	esp_err_t init_compensation_values();
	// Reads the raw pressure/temperature ADC registers and converts
	// them to compensated float values.
	esp_err_t read_data(float *temperature, float *pressure);
	float bmp280_compensate_temp(int32_t adc_T, int32_t *fine_temp) const;
	float bmp280_compensate_pressure(int32_t adc_P, int32_t fine_temp) const;
};

} // namespace svc::sensor

#endif // CPP_STARTER_BMP280_SERVICE_HPP

//
// Created by eugen on 8/8/2026.
//

#include "bmp280_service.hpp"

#include "domain/interface/ii2c_service.hpp"

#include <esp_log.h>
#include <etl/tuple.h>

using namespace svc::sensor;

Bmp280Service::Bmp280Service(II2CService &i2c_service) :
	i2c_svc_(i2c_service) {}

esp_err_t Bmp280Service::connect() {
	esp_err_t err = subscribe();
	if (err != ESP_OK) {
		return err;
	}
	return init_compensation_values();
}

Bmp280Service::~Bmp280Service() {
	esp_err_t err = i2c_svc_.unsubscribe(&bmp280_device_handle_);
	if (err != ESP_OK) {
		ESP_LOGE("i2c", "Failed to unsubscribe bmp280 device handle");
	}
	err = init_compensation_values();
	if (err != ESP_OK) {
		ESP_LOGE("i2c", "Failed to initialize bmp280 compensation values");
	}
}

esp_err_t Bmp280Service::subscribe() {
	uint8_t data[2] = {};
	esp_err_t err = i2c_svc_.subscribe(BMP280_ADDR, &bmp280_device_handle_);
	if (err != ESP_OK) {
		return err;
	}
	i2c_svc_.read(bmp280_device_handle_, BMP280_REG_ID, data, 1);
	ESP_LOGI("i2c", "WHO_AM_I = %X", data[0]);

	err = set_normal_sampling_mode();
	if (err != ESP_OK) {
		return err;
	}

	ESP_LOGI("i2c", "bmp280 successfully subscribed");
	return ESP_OK;
}

esp_err_t Bmp280Service::set_normal_sampling_mode() const {
	// Put the sensor into normal mode with x2 oversampling. (s 4.3.4, s 3.6, s 3.3.1, s 3.3.2)
	int data = 0b010 << 5 | 0b010 << 2 | 0b11; // osrs_t=001 (x1), osrs_p=001 (x1), mode=11 (normal)
	return i2c_svc_.write(bmp280_device_handle_, BMP280_REG_CTRL_MEAS,
						   data);
}

esp_err_t Bmp280Service::init_compensation_values() {

	// each config value is a short, i.e. 16 bits == 2 bytes (s 3.11.2).
	// 12 config values * 2 bytes = 24 bytes
	uint8_t buf[24] = {};
	esp_err_t err = i2c_svc_.read(bmp280_device_handle_, BMP280_REG_CALIB, buf,
								  sizeof(buf));
	if (err != ESP_OK) {
		return err;
	}

	// each calibration value is made up of MSB + LSB (s 3.11.2)
	// MSB is most significant byte, not bit.

	bmp280_calib_.dig_T1 = static_cast<uint16_t>(buf[1] << 8 | buf[0]);
	bmp280_calib_.dig_T2 = static_cast<int16_t>(buf[3] << 8 | buf[2]);
	bmp280_calib_.dig_T3 = static_cast<int16_t>(buf[5] << 8 | buf[4]);
	bmp280_calib_.dig_P1 = static_cast<uint16_t>(buf[7] << 8 | buf[6]);
	bmp280_calib_.dig_P2 = static_cast<int16_t>(buf[9] << 8 | buf[8]);
	bmp280_calib_.dig_P3 = static_cast<int16_t>(buf[11] << 8 | buf[10]);
	bmp280_calib_.dig_P4 = static_cast<int16_t>(buf[13] << 8 | buf[12]);
	bmp280_calib_.dig_P5 = static_cast<int16_t>(buf[15] << 8 | buf[14]);
	bmp280_calib_.dig_P6 = static_cast<int16_t>(buf[17] << 8 | buf[16]);
	bmp280_calib_.dig_P7 = static_cast<int16_t>(buf[19] << 8 | buf[18]);
	bmp280_calib_.dig_P8 = static_cast<int16_t>(buf[21] << 8 | buf[20]);
	bmp280_calib_.dig_P9 = static_cast<int16_t>(buf[23] << 8 | buf[22]);

	return ESP_OK;
}

etl::tuple<float, esp_err_t> Bmp280Service::bmp280_read_temp() {
	float temperature, pressure;
	esp_err_t err = bmp280_read_data(&temperature, &pressure);
	return {temperature, err};
}

etl::tuple<float, esp_err_t> Bmp280Service::bmp280_read_pressure() {
	float temperature, pressure;
	esp_err_t err = bmp280_read_data(&temperature, &pressure);
	return {pressure, err};
}

esp_err_t Bmp280Service::bmp280_read_data(float *temperature, float *pressure) {
	// 3 bytes for pressure, and another 3 bytes for temperature (s 4.3.6 -
	// s 4.3.7)
	uint8_t data[6];
	esp_err_t err =
		i2c_svc_.read(bmp280_device_handle_, BMP280_PRESSURE_REGISTER_ADDR,
					  data, sizeof(data));
	if (err != ESP_OK)
		return err;

	/* *
	 * (s 4.3.6 - s 4.3.7)
	 * MSB is most significant byte, not bit.
	 * bit:   19 18 17 16 15 14 13 12 | 11 10 9 8 7 6 5 4 | 3 2 1 0 0 0 0 0
			  └───── MSB (8 bits)  ─┘   └─ LSB (8 bits)─┘   └XLSB─┘
	 */

	// pressure = msb + lsb + xlsb (s 4.3.6)
	int32_t adc_P = (data[0]) << 12 | (data[1]) << 4 | (data[2] >> 4);

	// temperature = msb + lsb + xlsb (s 4.3.7)
	int32_t adc_T = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);

	int32_t fine_temp = 0;
	*temperature = bmp280_compensate_temp(adc_T, &fine_temp);
	*pressure = bmp280_compensate_pressure(adc_P, fine_temp);
	return ESP_OK;
}

// Compensation formula for temp copied from Bosch datasheet (s 3.11.3)
float Bmp280Service::bmp280_compensate_temp(int32_t adc_temp,
											int32_t *fine_temp) const {
	bmp280_calib_t dev_ = this->bmp280_calib_;
	auto dev = &dev_;

	int32_t var1, var2;
	var1 = ((((adc_temp >> 3) - ((int32_t)dev->dig_T1 << 1))) *
			(int32_t)dev->dig_T2) >>
		   11;
	var2 = (((((adc_temp >> 4) - (int32_t)dev->dig_T1) *
			  ((adc_temp >> 4) - (int32_t)dev->dig_T1)) >>
			 12) *
			(int32_t)dev->dig_T3) >>
		   14;

	*fine_temp = var1 + var2;
	int64_t T = (*fine_temp * 5 + 128) >>
				8; // temperature in hundredths of a degree Celsius
	return T / 100.0f;
}

// Compensation formula for pressure copied from Bosch datasheet (s 3.11.3)
float Bmp280Service::bmp280_compensate_pressure(int32_t adc_press,
												int32_t fine_temp) const {
	bmp280_calib_t dev_ = this->bmp280_calib_;
	auto dev = &dev_;
	int64_t var1, var2, p;

	var1 = (int64_t)fine_temp - 128000;
	var2 = var1 * var1 * (int64_t)dev->dig_P6;
	var2 = var2 + ((var1 * (int64_t)dev->dig_P5) << 17);
	var2 = var2 + (((int64_t)dev->dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t)dev->dig_P3) >> 8) +
		   ((var1 * (int64_t)dev->dig_P2) << 12);
	var1 = (((int64_t)1 << 47) + var1) * ((int64_t)dev->dig_P1) >> 33;

	if (var1 == 0) {
		return 0; // avoid exception caused by division by zero
	}

	p = 1048576 - adc_press;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = ((int64_t)dev->dig_P9 * (p >> 13) * (p >> 13)) >> 25;
	var2 = ((int64_t)dev->dig_P8 * p) >> 19;

	p = ((p + var1 + var2) >> 8) + ((int64_t)dev->dig_P7 << 4);
	p = (float)p / 256.0f / 100.0f; // convert Pa -> hPa
	return p;
}

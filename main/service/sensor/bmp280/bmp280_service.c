//
// Native C counterpart of bmp280_service.cpp.
//

#include "bmp280_service.h"

#include <esp_log.h>

#include <string.h>

static const char TAG[] = "bmp280";

// Downcast from the interface; valid because `base` is the first member.
#define BMP280_SELF(iface) ((bmp280_service_t *)(iface))

static esp_err_t vt_connect(ibmp280_service_t *iface) {
	return bmp280_service_connect(BMP280_SELF(iface));
}

static esp_err_t vt_read_thermal(ibmp280_service_t *iface,
								 bmp_280_read_info_t *out_info) {
	return bmp280_service_read_thermal(BMP280_SELF(iface), out_info);
}

static void vt_deinit(ibmp280_service_t *iface) {
	bmp280_service_deinit(BMP280_SELF(iface));
}

static const ibmp280_service_vtable_t BMP280_SERVICE_VTABLE = {
	.connect = vt_connect,
	.read_thermal = vt_read_thermal,
	.deinit = vt_deinit,
};

// Writes ctrl_meas to put the sensor into normal mode with x2 oversampling
// so it actually performs conversions (s 4.3.4, s 3.6, s 3.3.1, s 3.3.2).
static esp_err_t set_normal_sampling_mode(bmp280_service_t *self) {
	// osrs_t=010 (x2), osrs_p=010 (x2), mode=11 (normal)
	uint8_t ctrl = (uint8_t)(0b010 << 5 | 0b010 << 2 | 0b11);
	uint8_t write_buf[2] = {BMP280_REG_CTRL_MEAS, ctrl};
	return ii2c_service_write_buffer(self->i2c_svc, self->device_handle,
									 write_buf, sizeof(write_buf));
}

// Registers the BMP280 on the I2C bus and logs its WHO_AM_I value.
static esp_err_t subscribe(bmp280_service_t *self) {
	esp_err_t err = ii2c_service_subscribe(self->i2c_svc, BMP280_ADDR,
										   &self->device_handle);
	if (err != ESP_OK) {
		return err;
	}
	self->subscribed = true;

	uint8_t id = 0;
	ii2c_service_read(self->i2c_svc, self->device_handle, BMP280_REG_ID, &id,
					  1);
	ESP_LOGI(TAG, "WHO_AM_I = %X", id);

	err = set_normal_sampling_mode(self);
	if (err != ESP_OK) {
		return err;
	}

	ESP_LOGI(TAG, "bmp280 successfully subscribed");
	return ESP_OK;
}

// Reads the factory calibration registers into self->calib.
static esp_err_t init_compensation_values(bmp280_service_t *self) {
	// 12 calibration shorts * 2 bytes = 24 bytes (s 3.11.2)
	uint8_t buf[24] = {0};
	esp_err_t err = ii2c_service_read(self->i2c_svc, self->device_handle,
									  BMP280_REG_CALIB, buf, sizeof(buf));
	if (err != ESP_OK) {
		return err;
	}

	// each calibration value is LSB first, then MSB (s 3.11.2)
	bmp280_calib_t *c = &self->calib;
	c->dig_T1 = (uint16_t)(buf[1] << 8 | buf[0]);
	c->dig_T2 = (int16_t)(buf[3] << 8 | buf[2]);
	c->dig_T3 = (int16_t)(buf[5] << 8 | buf[4]);
	c->dig_P1 = (uint16_t)(buf[7] << 8 | buf[6]);
	c->dig_P2 = (int16_t)(buf[9] << 8 | buf[8]);
	c->dig_P3 = (int16_t)(buf[11] << 8 | buf[10]);
	c->dig_P4 = (int16_t)(buf[13] << 8 | buf[12]);
	c->dig_P5 = (int16_t)(buf[15] << 8 | buf[14]);
	c->dig_P6 = (int16_t)(buf[17] << 8 | buf[16]);
	c->dig_P7 = (int16_t)(buf[19] << 8 | buf[18]);
	c->dig_P8 = (int16_t)(buf[21] << 8 | buf[20]);
	c->dig_P9 = (int16_t)(buf[23] << 8 | buf[22]);

	return ESP_OK;
}

// Compensation formula for temperature, from the Bosch datasheet (s 3.11.3)
static float compensate_temp(const bmp280_service_t *self, int32_t adc_temp,
							 int32_t *fine_temp) {
	const bmp280_calib_t *dev = &self->calib;

	int32_t var1 = ((((adc_temp >> 3) - ((int32_t)dev->dig_T1 << 1))) *
					(int32_t)dev->dig_T2) >>
				   11;
	int32_t var2 = (((((adc_temp >> 4) - (int32_t)dev->dig_T1) *
					  ((adc_temp >> 4) - (int32_t)dev->dig_T1)) >>
					 12) *
					(int32_t)dev->dig_T3) >>
				   14;

	*fine_temp = var1 + var2;
	// temperature in hundredths of a degree Celsius
	int32_t t = (*fine_temp * 5 + 128) >> 8;
	return t / 100.0f;
}

// Compensation formula for pressure, from the Bosch datasheet (s 3.11.3)
static float compensate_pressure(const bmp280_service_t *self,
								 int32_t adc_press, int32_t fine_temp) {
	const bmp280_calib_t *dev = &self->calib;
	int64_t var1, var2, p;

	var1 = (int64_t)fine_temp - 128000;
	var2 = var1 * var1 * (int64_t)dev->dig_P6;
	var2 = var2 + ((var1 * (int64_t)dev->dig_P5) << 17);
	var2 = var2 + (((int64_t)dev->dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t)dev->dig_P3) >> 8) +
		   ((var1 * (int64_t)dev->dig_P2) << 12);
	var1 = (((int64_t)1 << 47) + var1) * ((int64_t)dev->dig_P1) >> 33;

	if (var1 == 0) {
		return 0; // avoid division by zero
	}

	p = 1048576 - adc_press;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = ((int64_t)dev->dig_P9 * (p >> 13) * (p >> 13)) >> 25;
	var2 = ((int64_t)dev->dig_P8 * p) >> 19;

	p = ((p + var1 + var2) >> 8) + ((int64_t)dev->dig_P7 << 4);
	return (float)p / 256.0f / 100.0f; // Pa -> hPa
}

void bmp280_service_init(bmp280_service_t *self, ii2c_service_t *i2c_svc) {
	memset(self, 0, sizeof(*self));
	self->base.vtable = &BMP280_SERVICE_VTABLE;
	self->i2c_svc = i2c_svc;
}

void bmp280_service_deinit(bmp280_service_t *self) {
	if (self->subscribed) {
		esp_err_t err =
			ii2c_service_unsubscribe(self->i2c_svc, &self->device_handle);
		if (err != ESP_OK) {
			ESP_LOGE(TAG, "Failed to unsubscribe bmp280 device handle");
		}
		self->subscribed = false;
	}
	memset(&self->calib, 0, sizeof(self->calib));
}

esp_err_t bmp280_service_connect(bmp280_service_t *self) {
	esp_err_t err = subscribe(self);
	if (err != ESP_OK) {
		return err;
	}
	return init_compensation_values(self);
}

esp_err_t bmp280_service_read_data(bmp280_service_t *self, float *temperature,
								   float *pressure) {
	// 3 bytes pressure then 3 bytes temperature (s 4.3.6 - s 4.3.7)
	uint8_t data[6];
	esp_err_t err =
		ii2c_service_read(self->i2c_svc, self->device_handle,
						  BMP280_PRESSURE_REGISTER_ADDR, data, sizeof(data));
	if (err != ESP_OK) {
		return err;
	}

	// value = msb[19:12] | lsb[11:4] | xlsb[3:0] (s 4.3.6 - s 4.3.7)
	int32_t adc_p = (int32_t)data[0] << 12 | (int32_t)data[1] << 4 |
					(int32_t)(data[2] >> 4);
	int32_t adc_t = (int32_t)data[3] << 12 | (int32_t)data[4] << 4 |
					(int32_t)(data[5] >> 4);

	int32_t fine_temp = 0;
	*temperature = compensate_temp(self, adc_t, &fine_temp);
	*pressure = compensate_pressure(self, adc_p, fine_temp);
	return ESP_OK;
}

esp_err_t bmp280_service_read_thermal(bmp280_service_t *self,
									  bmp_280_read_info_t *out_info) {
	float temperature = 0, pressure = 0;
	esp_err_t err = bmp280_service_read_data(self, &temperature, &pressure);
	out_info->temperature_celcius = (int16_t)temperature;
	out_info->relative_humidity = (int16_t)pressure;
	return err;
}

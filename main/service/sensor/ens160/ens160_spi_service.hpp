//
// Created by eugen on 9/4/2026.
//

#ifndef CPP_STARTER_ENS160_SPI_SERVICE_HPP
#define CPP_STARTER_ENS160_SPI_SERVICE_HPP
#include <iens160_service.hpp>
#include <ispi_service.hpp>

#define ENS160_AQI_REG 0x21	 // s 16.2.8
#define ENS160_READ_BIT 0x01 // s 14.2.3
#define ENS160_WRITE_BIT 0x00

#define ENS160_ADDR 0x52
#define ENS160_REG_ID 0x60		 // s 16.2.1
#define ENS160_OP_MODE_ADDR 0x10 // s 16.2.2
#define ENS160_NORMAL_MODE 0x02	 // s 16.2.2
#define ENS160_AQI_REG 0x21		 // s 16.2.8
#define ENS160_TEMP_ADDR 0x13
#define ENS160_HUMIDITY_ADDR 0x15
#define ENS160_OPMODE_RESET 0xF0
#define PIN_NUM_CS 5

namespace svc::sensor::ens160 {
class Ens160Service_SPI : public IEns160Service {
public:
	Ens160Service_SPI(ISPIService &spi_service);
	~Ens160Service_SPI() override;
	esp_err_t connect(const float *ambient_temp_celcius_opt,
					  const float *ambient_relative_humidity_opt) override;
	etl::tuple<ens_160_read_info_t, esp_err_t> read_air_data() override;

private:
	spi_device_handle_t ens160_device_handle_ = {};
	ISPIService &spi_svc_;
	esp_err_t set_normal_mode() const;
	esp_err_t set_compensation_values(const float *temp_celcius_opt,
									  const float *relative_humidity_opt) const;
};
} // namespace svc::sensor::ens160

#endif // CPP_STARTER_ENS160_SPI_SERVICE_HPP

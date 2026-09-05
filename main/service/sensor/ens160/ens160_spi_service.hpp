//
// Created by eugen on 9/4/2026.
//

#ifndef CPP_STARTER_ENS160_SPI_SERVICE_HPP
#define CPP_STARTER_ENS160_SPI_SERVICE_HPP
#include "ens160_common.hpp"
#include <iens160_service.hpp>
#include <ispi_service.hpp>

#define ENS160_READ_BIT 0x01 // s 14.2.3
#define ENS160_WRITE_BIT 0x00
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

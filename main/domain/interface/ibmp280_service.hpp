//
// Created by eugen on 8/29/2026.
//

#ifndef CPP_STARTER_IBMP280_SERVICE_HPP
#define CPP_STARTER_IBMP280_SERVICE_HPP

#include "domain/model/model.hpp"

#include <esp_err.h>
#include <etl/tuple.h>

class IBmp280Service {
public:
	virtual ~IBmp280Service() = default;

	virtual esp_err_t connect() = 0;
	virtual etl::tuple<bmp_280_read_info_t, esp_err_t> read_thermal() = 0;
};

#endif // CPP_STARTER_IBMP280_SERVICE_HPP
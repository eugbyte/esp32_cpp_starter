//
// Created by eugen on 7/6/2026.
//

#ifndef CPP_STARTER_INVS_SERVICE_H
#define CPP_STARTER_INVS_SERVICE_H

#include "esp_err.h"
#include <cstddef>
#include <etl/span.h>
#include <etl/string_view.h>
#include <etl/tuple.h>


class INvsService {
public:
	virtual ~INvsService() = default;
	etl::tuple<size_t, esp_err_t> get_item_size(etl::string_view ns_name,
												etl::string_view key);
	esp_err_t get_blob(etl::string_view ns_name, etl::string_view key,
					   void *out, size_t required_size);
	esp_err_t write_blob(etl::string_view ns_name, etl::string_view key,
						 void *data, size_t size);
};

#endif // CPP_STARTER_INVS_SERVICE_H

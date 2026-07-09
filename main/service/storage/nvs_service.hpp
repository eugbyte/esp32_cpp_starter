//
// Created by eugen on 7/5/2026.
//

#ifndef CPP_STARTER_NVS_SERVICE_H
#define CPP_STARTER_NVS_SERVICE_H

#include "domain/interface/invs_service.hpp"
#include <esp_err.h>
#include <etl/string_view.h>

namespace svc::storage {
// https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs/nvs_rw_value_cxx/main/nvs_value_example_main.cpp
class NvsService : public INvsService {
public:
	NvsService() = default;

	etl::tuple<size_t, esp_err_t> get_item_size(etl::string_view ns_name,
												etl::string_view key);
	esp_err_t get_blob(etl::string_view ns_name, etl::string_view key,
					   void *out, size_t required_size);
	esp_err_t write_blob(etl::string_view ns_name, etl::string_view key,
						 void *data, size_t size);

private:
	// NVS namespace and key names are limited to 15 characters + null
	// terminator by the underlying NVS implementation.
	static constexpr size_t kMaxNameLen = 16;

	// Copies (truncates if necessary) an etl::string_view into a
	// null-terminated buffer suitable for the C/C++ NVS APIs.
	static void to_cstr(etl::string_view sv, char *out, size_t out_size);
};

} // namespace svc::storage

#endif // CPP_STARTER_NVS_SERVICE_H

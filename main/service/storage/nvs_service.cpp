//
// Created by eugen on 7/5/2026.
//

#include <memory>
#include <span>

#include "esp_err.h"
#include "esp_system.h"
#include "nvs.h"
#include "nvs_handle.hpp"
#include <etl/span.h>
#include <etl/string_view.h>

class NvsService {
public:
	NvsService() = default;

	/**
	 *
	 * @param ns_name
	 * @param key
	 * @param out
	 * @return esp_err_t, which can be of values ESP_OK, ESP_ERR_NVS_NOT_FOUND, or other error codes.
	 */
	template <typename T>
	std::tuple<bool, esp_err_t> get_item(etl::span<T> out, etl::string_view ns_name, etl::string_view key) {
		esp_err_t err = ESP_OK;
		// Handle will automatically close when going out of scope or when it's reset.
		std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(ns_name.data(), NVS_READWRITE, &err);

		if (err != ESP_OK) {
			printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
			return {false, err};
		}

		printf("Done\n");
		err = handle->get_blob(key.data(), out.data(), out.size());
		switch (err) {
		case ESP_OK:
			return {true, ESP_OK};
		case ESP_ERR_NVS_NOT_FOUND:
			return {false, ESP_OK};
		default:
			printf("err not handled: %d", err);
		}
		return {false, err};
	}
};
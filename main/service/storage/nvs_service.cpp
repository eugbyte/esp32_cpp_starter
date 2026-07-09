//
// Created by eugen on 7/5/2026.
//

#include "nvs_service.hpp"

#include <cstring>
#include <esp_log.h>
#include <esp_system.h>
#include <etl/string.h>
#include <nvs.h>
#include <nvs_handle.hpp>

using etl::tuple;
using svc::storage::NvsService;

namespace {
constexpr const char *TAG = "NvsService";
}

void NvsService::to_cstr(etl::string_view sv, char *out, size_t out_size) {
	size_t len = sv.size() < (out_size - 1) ? sv.size() : (out_size - 1);
	std::memcpy(out, sv.data(), len);
	out[len] = '\0';
}

tuple<size_t, esp_err_t> NvsService::get_item_size(etl::string_view ns_name,
												   etl::string_view key) {
	char ns_buf[kMaxNameLen];
	char key_buf[kMaxNameLen];
	to_cstr(ns_name, ns_buf, sizeof(ns_buf));
	to_cstr(key, key_buf, sizeof(key_buf));

	esp_err_t err;
	std::unique_ptr<nvs::NVSHandle> handle =
		nvs::open_nvs_handle(ns_buf, NVS_READONLY, &err);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open namespace '%s' (%s)", ns_buf,
				 esp_err_to_name(err));
		return {0, err};
	}

	size_t required_size = 0;
	err = handle->get_item_size(nvs::ItemType::BLOB, key_buf, required_size);
	if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
		ESP_LOGE(TAG, "Failed to get size of '%s' (%s)", key_buf,
				 esp_err_to_name(err));
	}

	return {required_size, err};
}

esp_err_t NvsService::get_blob(etl::string_view ns_name, etl::string_view key,
							   void *out, size_t required_size) {
	char ns_buf[kMaxNameLen];
	char key_buf[kMaxNameLen];
	to_cstr(ns_name, ns_buf, sizeof(ns_buf));
	to_cstr(key, key_buf, sizeof(key_buf));

	esp_err_t err;
	std::unique_ptr<nvs::NVSHandle> handle =
		nvs::open_nvs_handle(ns_buf, NVS_READONLY, &err);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open namespace '%s' (%s)", ns_buf,
				 esp_err_to_name(err));
		return err;
	}

	err = handle->get_blob(key_buf, out, required_size);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to read blob '%s' (%s)", key_buf,
				 esp_err_to_name(err));
	}

	return err;
}

esp_err_t NvsService::write_blob(etl::string_view ns_name, etl::string_view key,
								 void *data, size_t size) {
	char ns_buf[kMaxNameLen];
	char key_buf[kMaxNameLen];
	to_cstr(ns_name, ns_buf, sizeof(ns_buf));
	to_cstr(key, key_buf, sizeof(key_buf));

	esp_err_t err;
	std::unique_ptr<nvs::NVSHandle> handle =
		nvs::open_nvs_handle(ns_buf, NVS_READWRITE, &err);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open namespace '%s' (%s)", ns_buf,
				 esp_err_to_name(err));
		return err;
	}

	err = handle->set_blob(key_buf, data, size);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to write blob '%s' (%s)", key_buf,
				 esp_err_to_name(err));
		return err;
	}

	err = handle->commit();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to commit NVS changes for '%s' (%s)", key_buf,
				 esp_err_to_name(err));
	}

	return err;
}

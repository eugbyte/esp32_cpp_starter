//
// Native C counterpart of nvs_service.cpp.
//

#include "nvs_service.h"

#include <esp_log.h>
#include <nvs.h>

#include <string.h>

static const char TAG[] = "nvs_service";

// Copies src into out as a null-terminated string, truncating to fit.
static void to_cstr(const char *src, char *out, size_t out_size) {
	size_t len = strlen(src);
	if (len > out_size - 1) {
		len = out_size - 1;
	}
	memcpy(out, src, len);
	out[len] = '\0';
}

// Opens ns_name with the given mode, logging on failure.
static esp_err_t open_namespace(const char *ns_name, nvs_open_mode_t mode,
								nvs_handle_t *out_handle) {
	char ns_buf[NVS_SERVICE_MAX_NAME_LEN];
	to_cstr(ns_name, ns_buf, sizeof(ns_buf));

	esp_err_t err = nvs_open(ns_buf, mode, out_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open namespace '%s' (%s)", ns_buf,
				 esp_err_to_name(err));
	}
	return err;
}

void nvs_service_init(nvs_service_t *self) { self->reserved = 0; }

void nvs_service_deinit(nvs_service_t *self) { (void)self; }

esp_err_t nvs_service_get_item_size(nvs_service_t *self, const char *ns_name,
									const char *key, size_t *out_size) {
	(void)self;
	*out_size = 0;

	char key_buf[NVS_SERVICE_MAX_NAME_LEN];
	to_cstr(key, key_buf, sizeof(key_buf));

	nvs_handle_t handle;
	esp_err_t err = open_namespace(ns_name, NVS_READONLY, &handle);
	if (err != ESP_OK) {
		return err;
	}

	// Passing a NULL buffer makes nvs_get_blob() report the stored size.
	size_t required_size = 0;
	err = nvs_get_blob(handle, key_buf, NULL, &required_size);
	if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
		ESP_LOGE(TAG, "Failed to get size of '%s' (%s)", key_buf,
				 esp_err_to_name(err));
	}
	nvs_close(handle);

	*out_size = required_size;
	return err;
}

esp_err_t nvs_service_get_blob(nvs_service_t *self, const char *ns_name,
							   const char *key, void *out,
							   size_t required_size) {
	(void)self;
	char key_buf[NVS_SERVICE_MAX_NAME_LEN];
	to_cstr(key, key_buf, sizeof(key_buf));

	nvs_handle_t handle;
	esp_err_t err = open_namespace(ns_name, NVS_READONLY, &handle);
	if (err != ESP_OK) {
		return err;
	}

	size_t length = required_size;
	err = nvs_get_blob(handle, key_buf, out, &length);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to read blob '%s' (%s)", key_buf,
				 esp_err_to_name(err));
	}
	nvs_close(handle);
	return err;
}

esp_err_t nvs_service_write_blob(nvs_service_t *self, const char *ns_name,
								 const char *key, const void *data,
								 size_t size) {
	(void)self;
	char key_buf[NVS_SERVICE_MAX_NAME_LEN];
	to_cstr(key, key_buf, sizeof(key_buf));

	nvs_handle_t handle;
	esp_err_t err = open_namespace(ns_name, NVS_READWRITE, &handle);
	if (err != ESP_OK) {
		return err;
	}

	err = nvs_set_blob(handle, key_buf, data, size);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to write blob '%s' (%s)", key_buf,
				 esp_err_to_name(err));
		nvs_close(handle);
		return err;
	}

	err = nvs_commit(handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to commit NVS changes for '%s' (%s)", key_buf,
				 esp_err_to_name(err));
	}
	nvs_close(handle);
	return err;
}

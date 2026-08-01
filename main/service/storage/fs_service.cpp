//
// Created by eugen on 7/31/2026.
//

#include "fs_service.hpp"

#include <esp_err.h>
#include <esp_littlefs.h>
#include <esp_log.h>

static const char *TAG = "fs_service";

// Mounts the "www" LittleFS partition at WEB_PAGE_MOUNT_POINT_IN_FS,
// formatting it if mounting fails, and logs partition usage.
esp_err_t svc::storage::init_fs() {
	esp_vfs_littlefs_conf_t conf = {};
	conf.base_path = WEB_PAGE_MOUNT_POINT_IN_FS;
	conf.partition_label = "www";
	conf.format_if_mount_failed = true;

	esp_err_t ret = esp_vfs_littlefs_register(&conf);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find LittleFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)",
					 esp_err_to_name(ret));
		}
		return ESP_FAIL;
	}

	size_t total = 0, used = 0;
	ret = esp_littlefs_info(conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s)",
				 esp_err_to_name(ret));
		esp_littlefs_format(conf.partition_label);
	} else {
		ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
	}

	return ESP_OK;
}
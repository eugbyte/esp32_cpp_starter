//
// Native C counterpart of fs_service.cpp.
//

#include "fs_service.h"

#include <esp_littlefs.h>
#include <esp_log.h>

#include <string.h>

static const char TAG[] = "fs_service";

esp_err_t fs_service_init(fs_service_t *self, const char *base_path) {
	strlcpy(self->base_path, base_path, sizeof(self->base_path));
	self->partition_label = FS_SERVICE_PARTITION_LABEL;
	self->mounted = false;

	esp_vfs_littlefs_conf_t conf = {
		.base_path = self->base_path,
		.partition_label = self->partition_label,
		.format_if_mount_failed = true,
	};

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
	self->mounted = true;

	size_t total = 0, used = 0;
	ret = esp_littlefs_info(self->partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s)",
				 esp_err_to_name(ret));
		esp_littlefs_format(self->partition_label);
	} else {
		ESP_LOGI(TAG, "Partition size: total: %d, used: %d", (int)total,
				 (int)used);
	}

	return ESP_OK;
}

void fs_service_deinit(fs_service_t *self) {
	if (self->mounted) {
		esp_vfs_littlefs_unregister(self->partition_label);
		self->mounted = false;
	}
}

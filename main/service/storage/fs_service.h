//
// Native C counterpart of fs_service.hpp.
//

#ifndef C_STARTER_FS_SERVICE_H
#define C_STARTER_FS_SERVICE_H

#include <esp_err.h>
#include <esp_vfs.h>
#include <stdbool.h>

#define WEB_PAGE_MOUNT_POINT_IN_FS "/www"
#define FS_SERVICE_PARTITION_LABEL "www"

// LittleFS mount for the web page partition.
typedef struct fs_service {
	char base_path[ESP_VFS_PATH_MAX + 1];
	const char *partition_label;
	bool mounted;
} fs_service_t;

// Mounts the "www" LittleFS partition at base_path, formatting it if
// mounting fails, and logs partition usage. Must be called before serving
// static files.
esp_err_t fs_service_init(fs_service_t *self, const char *base_path);
// Unmounts the partition if it was mounted.
void fs_service_deinit(fs_service_t *self);

#endif // C_STARTER_FS_SERVICE_H

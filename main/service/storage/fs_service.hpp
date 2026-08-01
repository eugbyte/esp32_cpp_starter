//
// Created by eugen on 7/31/2026.
//

#ifndef CPP_STARTER_FS_SERVICE_HPP
#define CPP_STARTER_FS_SERVICE_HPP

#define WEB_PAGE_MOUNT_POINT_IN_FS "/www"
#include <esp_err.h>

namespace svc::storage {
// Mounts the web page LittleFS partition; must be called before
// serving static files.
esp_err_t init_fs();
}

#endif // CPP_STARTER_FS_SERVICE_HPP

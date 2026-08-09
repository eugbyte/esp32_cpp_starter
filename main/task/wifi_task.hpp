//
// Created by eugen on 8/9/2026.
//

#ifndef CPP_STARTER_WIFI_TASK_HPP
#define CPP_STARTER_WIFI_TASK_HPP
#include "service/wifi/wifi_service.hpp"

#include <esp_err.h>

namespace task {
esp_err_t wifi_task(svc::wifi::WifiService &wifi_service);

} // namespace task

#endif // CPP_STARTER_WIFI_TASK_HPP

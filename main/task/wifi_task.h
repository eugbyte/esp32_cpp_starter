//
// Native C counterpart of wifi_task.hpp.
//

#ifndef C_STARTER_WIFI_TASK_H
#define C_STARTER_WIFI_TASK_H

#include "service/wifi/wifi_service.h"

#include <esp_err.h>

// Brings up Wi-Fi using an initialized Wi-Fi service.
typedef struct wifi_task {
	wifi_service_t *wifi_service;
} wifi_task_t;

void wifi_task_init(wifi_task_t *self, wifi_service_t *wifi_service);
void wifi_task_deinit(wifi_task_t *self);
// Starts the software AP (WIFI_MODE_APSTA). Connecting to a STA access
// point is left commented out, as in the C++ task.
esp_err_t wifi_task_start(wifi_task_t *self);

#endif // C_STARTER_WIFI_TASK_H

//
// Native C counterpart of web_handler.hpp.
//

#ifndef C_STARTER_WEB_HANDLER_H
#define C_STARTER_WEB_HANDLER_H

#include "domain/interface/ildc_service.h"
#include "domain/interface/invs_service.h"
#include "domain/interface/iwifi_service.h"

#include <esp_http_server.h>
#include <esp_vfs.h>

#define SCRATCH_BUFSIZE (10240)
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)

/**
 * HTTP route handlers bound to the shared LCD/NVS/Wi-Fi services.
 *
 * Routes are registered by the HTTP task, which passes the instance as each
 * route's user_ctx, so it must outlive the HTTP server. The struct
 * holds a SCRATCH_BUFSIZE file buffer, so give it static storage or
 * allocate it on the heap rather than the stack.
 */
typedef struct web_handler {
	ilcd_service_t *lcd_svc;
	invs_service_t *nvs_svc;
	iwifi_service_t *wifi_svc;
	char base_path[ESP_VFS_PATH_MAX + 1];
	char scratch[SCRATCH_BUFSIZE];
} web_handler_t;

// Binds the handler to the services used by its routes and to the
// filesystem base_path that static files are served from.
void web_handler_init(web_handler_t *self, ilcd_service_t *lcd_svc,
					  invs_service_t *nvs_svc, iwifi_service_t *wifi_svc,
					  const char *base_path);
void web_handler_deinit(web_handler_t *self);

// GET /health: returns a static JSON OK payload.
esp_err_t web_handler_healthcheck(web_handler_t *self, httpd_req_t *req);
// GET /*: serves files from base_path, defaulting to index.html for
// directory-style URIs and unknown paths (SPA routing).
esp_err_t web_handler_serve_static_files(web_handler_t *self,
										 httpd_req_t *req);
// POST /wifi/login: parses ssid/password from the JSON body and persists
// them to NVS under the "wifi" namespace.
esp_err_t web_handler_login(web_handler_t *self, httpd_req_t *req);

#endif // C_STARTER_WEB_HANDLER_H

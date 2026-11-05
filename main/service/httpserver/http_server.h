//
// Native C counterpart of http_server.hpp.
//

#ifndef C_STARTER_HTTP_SERVER_H
#define C_STARTER_HTTP_SERVER_H

#include <esp_http_server.h>

// HTTP Server task
#define HTTP_SERVER_TASK_STACK_SIZE 8192
#define HTTP_SERVER_TASK_PRIORITY 4
#define HTTP_SERVER_TASK_CORE_ID 0

// Usage order:
//   1. http_server_init() to build the config.
//   2. http_server_init_mdns() (optional) to advertise http://esp32.local.
//   3. http_server_start() to bind and start httpd.
//   4. http_server_register_route() once per route, after
//      http_server_start() has created the server handle.
//
// The Wi-Fi service (STA and/or AP) must already be connected/started
// before http_server_start()/http_server_init_mdns(), and esp_netif_init()
// plus the default event loop must already be initialized.
typedef struct http_server {
	httpd_handle_t server_handle;
	httpd_config_t config;
} http_server_t;

// Builds the httpd config (core, priority, stack size, timeouts, etc).
// Does not require Wi-Fi to be up yet.
void http_server_init(http_server_t *self);
// Stops the server if running.
void http_server_deinit(http_server_t *self);
// Starts the httpd instance.
esp_err_t http_server_start(http_server_t *self);
// Registers a URI handler. Call after http_server_start().
esp_err_t http_server_register_route(http_server_t *self,
									 const httpd_uri_t *route_handler);
// Starts mDNS and advertises the device as http://esp32.local. Independent
// of the httpd instance.
esp_err_t http_server_init_mdns(http_server_t *self);

#endif // C_STARTER_HTTP_SERVER_H

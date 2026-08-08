//
// Created by eugen on 7/11/2026.
//

#ifndef CPP_STARTER_HTTP_SERVER_HPP
#define CPP_STARTER_HTTP_SERVER_HPP

#include <esp_http_server.h>
#include <esp_log.h>

// HTTP Server task
#define HTTP_SERVER_TASK_STACK_SIZE 8192
#define HTTP_SERVER_TASK_PRIORITY 4
#define HTTP_SERVER_TASK_CORE_ID 0

namespace svc::httpserver {
// Usage order:
//   1. HttpServer() to build the config.
//   2. init_mdns() (optional) to advertise http://esp32.local.
//   3. start_server() to bind and start httpd.
//   4. register_route() once per route, after start_server() has
//      created the server handle.
//
// WifiService (STA and/or AP) must already be connected/started
// before start_server()/init_mdns() are called: the underlying
// esp_netif/Wi-Fi interfaces need to be up for the server to be
// reachable and for mDNS to advertise over them. esp_netif_init()
// and the default event loop must also already be initialized.
class HttpServer {
private:
	httpd_handle_t server_handle_ = nullptr;
	httpd_config_t config_ = HTTPD_DEFAULT_CONFIG();

public:
	// Builds the httpd config (core, priority, stack size, timeouts, etc).
	// Does not require Wi-Fi to be up yet.
	HttpServer();
	// Stops the server if running.
	~HttpServer();

	// Starts the httpd instance. Call after Wi-Fi (STA/AP) is
	// connected/started so the server is actually reachable.
	esp_err_t start_server();
	// Registers a URI handler. Call after start_server(), since it
	// needs the server handle created there.
	esp_err_t register_route(const httpd_uri_t *route_handler);
	// Starts mDNS and advertises the device as http://esp32.local.
	// Call after Wi-Fi (STA/AP) is connected/started, and can be
	// called before or after start_server()/register_route() since
	// it operates independently of the httpd instance.
	esp_err_t init_mdns();
};

} // namespace svc::httpserver

#endif // CPP_STARTER_HTTP_SERVER_HPP

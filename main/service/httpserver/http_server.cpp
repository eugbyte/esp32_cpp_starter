//
// Created by eugen on 7/11/2026.
//

#include "http_server.hpp"
#include <esp_netif.h>	// must include esp_netif.h before mdns.h
#include <mdns.h>

using namespace svc::httpserver;

static const char TAG[] = "http_server";

HttpServer::HttpServer() {
	// the core that the HTTP server will run on
	config_.core_id = HTTP_SERVER_TASK_CORE_ID;
	// Adjust the default priority to 1 less than the wifi application task
	config_.task_priority = HTTP_SERVER_TASK_PRIORITY;
	// Bump up the stack size
	config_.stack_size = HTTP_SERVER_TASK_STACK_SIZE;
	// increase uri handlers
	config_.max_uri_handlers = 20;
	config_.uri_match_fn = httpd_uri_match_wildcard;

	// increase timeout limits
	config_.recv_wait_timeout = 10;
	config_.send_wait_timeout = 10;

	ESP_LOGI(TAG,
			 "http_server_configure: Starting server on port: '%d' with task "
			 "priority: '%d'",
			 config_.server_port, config_.task_priority);
}

// Stops the httpd instance if one was started.
HttpServer::~HttpServer() {
	if (server_handle_ != nullptr) {
		httpd_stop(server_handle_);
	}
}

// Starts the httpd instance using the config built in the constructor.
// Call once WifiService (STA/AP) is connected/started, otherwise the
// server has nothing to bind to that clients can reach. Must be
// called before register_route().
esp_err_t HttpServer::start_server() {
	return httpd_start(&server_handle_, &config_);
}

// Registers a URI handler with the running httpd instance. Must be
// called after start_server(), since it needs the server handle
// created there.
esp_err_t HttpServer::register_route(const httpd_uri_t *route_handler) {
	return httpd_register_uri_handler(server_handle_, route_handler);
}

// Initializes mDNS and advertises the device as http://esp32.local
// with an _http._tcp service record. Call once WifiService (STA/AP)
// is connected/started so the advertisement goes out over an active
// interface; independent of start_server()/register_route(), so it
// can be called before or after those.
esp_err_t HttpServer::init_mdns() {
	esp_err_t err = mdns_init();
	if (err) {
		printf("mDNS init failed: %d\n", err);
		return err;
	}

	mdns_hostname_set("esp32"); // device reachable as http://esp32.local
	mdns_instance_name_set("esp32 web server");

	// Optionally advertise services (e.g. HTTP server on port 80)
	mdns_service_add(nullptr, "_http", "_tcp", 80, nullptr, 0);
	return ESP_OK;
}
//
// Native C counterpart of http_server.cpp.
//

#include "http_server.h"

#include <esp_log.h>
#include <esp_netif.h> // must include esp_netif.h before mdns.h
#include <mdns.h>

static const char TAG[] = "http_server";

void http_server_init(http_server_t *self) {
	self->server_handle = NULL;
	self->config = (httpd_config_t)HTTPD_DEFAULT_CONFIG();

	// the core that the HTTP server will run on
	self->config.core_id = HTTP_SERVER_TASK_CORE_ID;
	// 1 less than the wifi application task
	self->config.task_priority = HTTP_SERVER_TASK_PRIORITY;
	self->config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;
	self->config.max_uri_handlers = 20;
	self->config.uri_match_fn = httpd_uri_match_wildcard;
	self->config.recv_wait_timeout = 10;
	self->config.send_wait_timeout = 10;

	ESP_LOGI(TAG,
			 "http_server_configure: Starting server on port: '%d' with task "
			 "priority: '%d'",
			 self->config.server_port, (int)self->config.task_priority);
}

void http_server_deinit(http_server_t *self) {
	if (self->server_handle != NULL) {
		httpd_stop(self->server_handle);
		self->server_handle = NULL;
	}
}

esp_err_t http_server_start(http_server_t *self) {
	return httpd_start(&self->server_handle, &self->config);
}

esp_err_t http_server_register_route(http_server_t *self,
									 const httpd_uri_t *route_handler) {
	return httpd_register_uri_handler(self->server_handle, route_handler);
}

esp_err_t http_server_init_mdns(http_server_t *self) {
	(void)self;
	esp_err_t err = mdns_init();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "mDNS init failed: %d", err);
		return err;
	}

	mdns_hostname_set("esp32"); // device reachable as http://esp32.local
	mdns_instance_name_set("esp32 web server");

	// Advertise the HTTP server on port 80
	mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
	return ESP_OK;
}

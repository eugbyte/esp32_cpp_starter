//
// Native C counterpart of http_task.cpp.
//

#include "http_task.h"

#include <esp_log.h>

static const char TAG[] = "http_task";

// httpd handlers only receive the request, so the web handler instance
// travels in req->user_ctx; these adapt it to the web_handler_* methods.
static esp_err_t healthcheck_handler(httpd_req_t *req) {
	return web_handler_healthcheck((web_handler_t *)req->user_ctx, req);
}

static esp_err_t login_handler(httpd_req_t *req) {
	return web_handler_login((web_handler_t *)req->user_ctx, req);
}

static esp_err_t static_files_handler(httpd_req_t *req) {
	return web_handler_serve_static_files((web_handler_t *)req->user_ctx, req);
}

void http_task_init(http_task_t *self, web_handler_t *web_handler,
					http_server_t *http_server) {
	self->web_handler = web_handler;
	self->http_server = http_server;
	self->fs = (fs_service_t){0};
}

void http_task_deinit(http_task_t *self) {
	http_server_deinit(self->http_server);
	fs_service_deinit(&self->fs);
}

esp_err_t http_task_start(http_task_t *self) {
	// Serve files from the same path the web handler reads from.
	esp_err_t err = fs_service_init(&self->fs, self->web_handler->base_path);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to mount the web filesystem");
	}

	// device reachable at http://esp32.local
	err = http_server_init_mdns(self->http_server);
	if (err != ESP_OK) {
		return err;
	}
	err = http_server_start(self->http_server);
	if (err != ESP_OK) {
		return err;
	}

	// httpd copies each descriptor, so stack-allocated ones are fine.
	// The web handler must outlive the server, since it is the user_ctx.
	httpd_uri_t healthcheck_uri = {.uri = "/health",
								   .method = HTTP_GET,
								   .handler = healthcheck_handler,
								   .user_ctx = self->web_handler};
	http_server_register_route(self->http_server, &healthcheck_uri);

	httpd_uri_t login_uri = {.uri = "/wifi/login",
							 .method = HTTP_POST,
							 .handler = login_handler,
							 .user_ctx = self->web_handler};
	http_server_register_route(self->http_server, &login_uri);

	// wildcard route must be registered last
	httpd_uri_t common_get_uri = {.uri = "/*",
								  .method = HTTP_GET,
								  .handler = static_files_handler,
								  .user_ctx = self->web_handler};
	http_server_register_route(self->http_server, &common_get_uri);
	return ESP_OK;
}

//
// Created by eugen on 8/9/2026.
//

#include "http_task.hpp"

#include "service/httpserver/http_server.hpp"
#include "service/httpserver/web_handler.hpp"
#include "service/storage/fs_service.hpp"

#include <etl/string.h>

using namespace svc::httpserver;

esp_err_t task::http_task(WebHandler &web_handler, HttpServer &http_server) {
	etl::string<16> base_path = "/www";
	svc::storage::init_fs(base_path);

	// device reachable at http://esp32.local
	esp_err_t err = http_server.init_mdns();
	if (err != ESP_OK) {
		return err;
	}
	err = http_server.start_server();
	if (err != ESP_OK) {
		return err;
	}

	httpd_uri_t healthcheck_uri = {.uri = "/health",
								   .method = HTTP_GET,
								   .handler = WebHandler::healthcheck,
								   .user_ctx = nullptr};
	http_server.register_route(&healthcheck_uri);

	httpd_uri_t login_uri = {.uri = "/wifi/login",
							 .method = HTTP_POST,
							 .handler = [](httpd_req_t *req) -> esp_err_t {
								 const auto wh =
									 static_cast<WebHandler *>(req->user_ctx);
								 return wh->login(req);
							 },
							 .user_ctx = &web_handler};
	http_server.register_route(&login_uri);

	// wildcard route must be registered last
	// have req_ctx persist for the lifetime of the server, since it is used as
	// the user_ctx for the wildcard route handler
	static rest_server_context_t req_ctx{};
	strlcpy(req_ctx.base_path, base_path.data(), sizeof(req_ctx.base_path));
	httpd_uri_t common_get_uri = {.uri = "/*",
								  .method = HTTP_GET,
								  .handler = WebHandler::serve_static_files,
								  .user_ctx = &req_ctx};
	http_server.register_route(&common_get_uri);
	return ESP_OK;
}
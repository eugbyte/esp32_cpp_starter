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
class HttpServer {
private:
	httpd_handle_t server_handle_ = nullptr;
	httpd_config_t config_ = HTTPD_DEFAULT_CONFIG();

public:
	HttpServer();
	~HttpServer();

	esp_err_t start_server();
	esp_err_t register_route(const httpd_uri_t* route_handler);
	esp_err_t init_mdns();
};

}

#endif // CPP_STARTER_HTTP_SERVER_HPP

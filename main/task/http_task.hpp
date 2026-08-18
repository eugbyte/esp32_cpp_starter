//
// Created by eugen on 8/9/2026.
//

#include "service/httpserver/http_server.hpp"
#include "service/httpserver/web_handler.hpp"

#include <esp_err.h>

#ifndef CPP_STARTER_HTTP_TASK_HPP
#define CPP_STARTER_HTTP_TASK_HPP

namespace task {
// Mounts the web filesystem, starts mDNS and the HTTP server, and
// registers the /health, /wifi/login and static-file (/*) routes.
esp_err_t http_task(svc::httpserver::WebHandler &web_handler,
					svc::httpserver::HttpServer &http_server);
} // namespace task

#endif // CPP_STARTER_HTTP_TASK_HPP

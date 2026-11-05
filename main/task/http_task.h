//
// Native C counterpart of http_task.hpp.
//

#ifndef C_STARTER_HTTP_TASK_H
#define C_STARTER_HTTP_TASK_H

#include "service/httpserver/http_server.h"
#include "service/httpserver/web_handler.h"
#include "service/storage/fs_service.h"

#include <esp_err.h>

// Brings up the web stack: mounts the web filesystem, starts mDNS and the
// HTTP server, and registers the routes served by the web handler. The server keeps
// pointers to the web handler, so this struct and the objects it points to
// must outlive the server (give them static storage).
typedef struct http_task {
	web_handler_t *web_handler;
	http_server_t *http_server;
	fs_service_t fs;
} http_task_t;

// Binds the task to an initialized web handler and HTTP server.
void http_task_init(http_task_t *self, web_handler_t *web_handler,
					http_server_t *http_server);
// Stops the HTTP server and unmounts the filesystem.
void http_task_deinit(http_task_t *self);
// Mounts the filesystem at the web handler's base path, starts mDNS and the
// HTTP server, and registers the /health, /wifi/login and static-file (/*)
// routes.
esp_err_t http_task_start(http_task_t *self);

#endif // C_STARTER_HTTP_TASK_H

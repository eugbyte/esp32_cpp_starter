//
// Created by eugen on 7/11/2026.
//

#ifndef CPP_STARTER_WEB_HANDLER_HPP
#define CPP_STARTER_WEB_HANDLER_HPP

#include "domain/interface/ildc_service.hpp"
#include "domain/interface/invs_service.hpp"
#include "domain/interface/iwifi_service.hpp"
#include <esp_http_server.h>

namespace svc::httpserver {
class WebHandler {
private:
	ILcdService &lcd_svc_;
	INvsService &nvs_svc_;
	IWifiService &wifi_svc_;

public:
	WebHandler(ILcdService &lcd_svc, INvsService &nvs_svc,
			   IWifiService &wifi_svc);

	esp_err_t httpserver_healthcheck(httpd_req_t *req);
};
};

#endif // CPP_STARTER_WEB_HANDLER_HPP

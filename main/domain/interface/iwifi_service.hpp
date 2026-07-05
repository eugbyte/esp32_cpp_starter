//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_STARTER_IWIFISERVICE_H
#define CPP_STARTER_IWIFISERVICE_H
#include <esp_err.h>
#include <etl/string_view.h>

class IWifiService {
public:
	virtual ~IWifiService() = default;
	virtual esp_err_t sta_connect(etl::string_view ssid,
								  etl::string_view password) = 0;
	virtual esp_err_t sta_disconnect() = 0;
};

#endif // CPP_STARTER_IWIFISERVICE_H

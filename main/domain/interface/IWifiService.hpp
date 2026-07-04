//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_STARTER_IWIFISERVICE_H
#define CPP_STARTER_IWIFISERVICE_H
#include "etl/string_view.h"
#include <esp_err.h>

class IWifiService {
public:
	virtual ~IWifiService();
	virtual esp_err_t StaConnect(etl::string_view ssid,
								 etl::string_view password);
	virtual esp_err_t StaDisconnect();
};

#endif // CPP_STARTER_IWIFISERVICE_H

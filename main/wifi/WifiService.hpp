//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_STARTER_WIFISERVICE_H
#define CPP_STARTER_WIFISERVICE_H

#include "domain/interface/IWifiService.hpp"
#include "etl/string_view.h"

#include <esp_err.h>
#include <esp_netif_types.h>
#include <freertos/event_groups.h>

class WifiService : public IWifiService {
public:
	WifiService();
	~WifiService() override;
	esp_err_t StaConnect(etl::string_view ssid,
						 etl::string_view password) override;
	esp_err_t StaDisconnect() override;

private:
	esp_netif_t *esp_sta_ = nullptr;
	int s_retry_num_ = 0;
	EventGroupHandle_t s_wifi_event_group_{};
	esp_event_handler_instance_t instance_any_id_{};
	esp_event_handler_instance_t instance_got_ip_{};
	void StaEventHandler(void *arg, esp_event_base_t event_base,
						 int32_t event_id, void *event_data);
};

#endif // CPP_STARTER_WIFISERVICE_H

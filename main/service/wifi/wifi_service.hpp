//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_STARTER_WIFISERVICE_H
#define CPP_STARTER_WIFISERVICE_H

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include <esp_err.h>
#include <esp_netif_types.h>
#include <etl/string_view.h>

#include "domain/interface/iwifi_service.hpp"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define CONFIG_ESP_MAXIMUM_RETRY 5

namespace svc::wifi {

class WifiService : public IWifiService {
public:
	WifiService();
	~WifiService() override;
	esp_err_t sta_connect(etl::string_view ssid,
						  etl::string_view password) override;
	esp_err_t sta_disconnect() override;

private:
	esp_netif_t *esp_sta_ = nullptr;
	int s_retry_num_ = 0;
	EventGroupHandle_t s_wifi_event_group_{};
	esp_event_handler_instance_t instance_any_id_{};
	esp_event_handler_instance_t instance_got_ip_{};
	void StaEventHandler(void *arg, esp_event_base_t event_base,
						 int32_t event_id, void *event_data);
};

} // namespace svc::wifi

#endif // CPP_STARTER_WIFISERVICE_H

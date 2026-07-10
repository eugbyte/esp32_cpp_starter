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

/**
 * @brief ESP-IDF Wi-Fi STA implementation of IWifiService.
 *
 * Wraps esp_wifi station APIs and uses a FreeRTOS event group plus
 * WIFI_EVENT/IP_EVENT handlers to signal connect success/failure,
 * with automatic reconnect retries up to CONFIG_ESP_MAXIMUM_RETRY.
 */
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

	// WIFI_EVENT/IP_EVENT callback: triggers (re)connect attempts on
	// disconnect (up to CONFIG_ESP_MAXIMUM_RETRY) and sets
	// WIFI_CONNECTED_BIT/WIFI_FAIL_BIT on s_wifi_event_group_ once the
	// outcome is known.
	void StaEventHandler(void *arg, esp_event_base_t event_base,
						 int32_t event_id, void *event_data);
};

} // namespace svc::wifi

#endif // CPP_STARTER_WIFISERVICE_H

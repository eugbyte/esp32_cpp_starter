//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_STARTER_WIFISERVICE_H
#define CPP_STARTER_WIFISERVICE_H

#include "domain/interface/iwifi_service.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include <esp_err.h>
#include <esp_netif_types.h>
#include <etl/string_view.h>

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define CONFIG_ESP_MAXIMUM_RETRY 5

#define WIFI_AP_SSID "ESP32_AP"		// AP name
#define WIFI_AP_PASSWORD "password" // AP password
#define WIFI_AP_CHANNEL 1			// AP channel
#define WIFI_AP_SSID_HIDDEN 0		// AP visibility
#define WIFI_AP_MAX_CONNECTIONS 5	// AP max clients
#define WIFI_AP_BEACON_INTERVAL 100 // AP beacon: 100 milliseconds recommended
#define WIFI_AP_IP "192.168.0.1"	// AP default IP
#define WIFI_AP_GATEWAY                                                        \
	"192.168.0.1" // AP default Gateway (should be the same as the IP)
#define WIFI_AP_NETMASK "255.255.255.0" // AP netmask
#define WIFI_AP_BANDWIDTH                                                      \
	WIFI_BW20 // AP bandwidth 20 MHz (40 MHz is the other option)
#define WIFI_STA_POWER_SAVE WIFI_PS_NONE // Power save not used
#define MAX_SSID_LENGTH 32				 // IEEE standard maximum
#define MAX_PASSWORD_LENGTH 64			 // IEEE standard maximum
#define MAX_CONNECTION_RETRIES 5		 // Retry number on disconnect

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
	esp_err_t ap_connect();
	esp_err_t sta_connect(etl::string_view ssid,
						  etl::string_view password) override;
	esp_err_t sta_disconnect() override;

private:
	esp_netif_t *esp_sta_ = nullptr;
	esp_netif_t *esp_ap_ = nullptr;
	int s_retry_num_ = 0;
	EventGroupHandle_t s_wifi_event_group_{};
	esp_event_handler_instance_t instance_any_handler_id_{};
	esp_event_handler_instance_t instance_got_ip_handler_id_{};
	esp_err_t ap_event_handler(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data);
	// WIFI_EVENT/IP_EVENT callback: triggers (re)connect attempts on
	// disconnect (up to CONFIG_ESP_MAXIMUM_RETRY) and sets
	// WIFI_CONNECTED_BIT/WIFI_FAIL_BIT on s_wifi_event_group_ once the
	// outcome is known.
	void sta_event_handler(void *arg, esp_event_base_t event_base,
						   int32_t event_id, void *event_data);
};

} // namespace svc::wifi

#endif // CPP_STARTER_WIFISERVICE_H

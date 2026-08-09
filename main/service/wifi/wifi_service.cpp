//
// Created by eugen on 7/4/2026.
//

#include "wifi_service.hpp"

#include <cstring>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_wifi_types_generic.h>
#include <lwip/netdb.h>
#include <rom/ets_sys.h>

using svc::wifi::WifiService;

const char TAG[] = "WifiService";

// Destroys the default STA netif created in the constructor.
WifiService::~WifiService() {
	esp_netif_destroy_default_wifi(esp_sta_);
	esp_netif_destroy_default_wifi(esp_ap_);
}

// Creates the default STA netif, initializes the Wi-Fi driver (RAM
// storage only), and registers StaEventHandler for WIFI_EVENT/IP_EVENT.
WifiService::WifiService() {
	s_wifi_event_group_ = xEventGroupCreate();

	esp_sta_ = esp_netif_create_default_wifi_sta();
	esp_ap_ = esp_netif_create_default_wifi_ap();

	const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	esp_event_handler_t event_handler = [](void *arg,
										   esp_event_base_t event_base,
										   int32_t event_id, void *event_data) {
		auto *self = static_cast<WifiService *>(arg);
		self->sta_event_handler(arg, event_base, event_id, event_data);
		self->ap_event_handler(arg, event_base, event_id, event_data);
	};

	esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
										event_handler, this,
										&instance_any_handler_id_);
	esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
										event_handler, this,
										&instance_got_ip_handler_id_);
}

// Configures and starts the software access point (static IP/DHCP
// server, SSID/password/channel from the WIFI_AP_* macros) alongside
// the STA interface, switching the driver into WIFI_MODE_APSTA.
esp_err_t WifiService::ap_connect() {
	wifi_config_t ap_config = {};

	const size_t ssid_length =
		std::min(strlen(WIFI_AP_SSID), sizeof(ap_config.ap.ssid));
	std::memcpy(ap_config.ap.ssid, WIFI_AP_SSID, ssid_length);

	const size_t password_length =
		std::min(strlen(WIFI_AP_PASSWORD), sizeof(ap_config.ap.password));
	std::memcpy(ap_config.ap.password, WIFI_AP_PASSWORD, password_length);

	ap_config.ap.ssid_len = static_cast<uint8_t>(strlen(WIFI_AP_SSID));
	ap_config.ap.channel = WIFI_AP_CHANNEL;
	ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
	ap_config.ap.ssid_hidden = WIFI_AP_SSID_HIDDEN;
	ap_config.ap.max_connection = WIFI_AP_MAX_CONNECTIONS;
	ap_config.ap.beacon_interval = WIFI_AP_BEACON_INTERVAL;

	// Configure DHCP for the AP
	esp_netif_ip_info_t ap_ip_info = {};

	esp_netif_dhcps_stop(esp_ap_); ///> must call this first
	inet_pton(
		AF_INET, WIFI_AP_IP,
		&ap_ip_info.ip); ///> Assign access point's static IP, GW, and netmask
	inet_pton(AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw);
	inet_pton(AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask);
	ESP_ERROR_CHECK(esp_netif_set_ip_info(
		esp_ap_,
		&ap_ip_info)); ///> Statically configure the network interface
	ESP_ERROR_CHECK(esp_netif_dhcps_start(
		esp_ap_)); ///> Start the AP DHCP server (for connecting stations
	/// e.g. your mobile device)

	ESP_ERROR_CHECK(esp_wifi_set_mode(
		WIFI_MODE_APSTA)); ///> Setting the mode as Access Point / Station Mode
	ESP_ERROR_CHECK(
		esp_wifi_set_config(WIFI_IF_AP, &ap_config)); ///> Set our configuration
	ESP_ERROR_CHECK(esp_wifi_set_bandwidth(
		WIFI_IF_AP, WIFI_AP_BANDWIDTH)); ///> Our default bandwidth 20 MHz
	ESP_ERROR_CHECK(
		esp_wifi_set_ps(WIFI_STA_POWER_SAVE)); ///> Power save set to "NONE"
	ESP_ERROR_CHECK(esp_wifi_start());

	return ESP_OK;
}

// WIFI_EVENT callback for the AP interface: logs station join/leave
// events (WIFI_EVENT_AP_STACONNECTED/STADISCONNECTED).
esp_err_t WifiService::ap_event_handler(void *arg, esp_event_base_t event_base,
										int32_t event_id, void *event_data) {
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
		auto *event = static_cast<wifi_event_ap_staconnected_t *>(event_data);
		ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac),
				 event->aid);
	} else if (event_base == WIFI_EVENT &&
			   event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		auto *event =
			static_cast<wifi_event_ap_stadisconnected_t *>(event_data);
		ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d, reason=%d",
				 MAC2STR(event->mac), event->aid, event->reason);
	}

	return ESP_OK;
}

// Configures STA mode with the given credentials, starts Wi-Fi, and
// blocks until StaEventHandler reports connected or failed/exhausted
// retries via s_wifi_event_group_.
esp_err_t WifiService::sta_connect(etl::string_view ssid,
								   etl::string_view password) {
	wifi_config_t wifi_config = {};
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

	const size_t ssid_length =
		std::min(ssid.length(), sizeof(wifi_config.sta.ssid) - 1);
	std::memcpy(wifi_config.sta.ssid, ssid.data(), ssid_length);
	wifi_config.sta.ssid[ssid_length] = '\0';

	const size_t password_length =
		std::min(password.length(), sizeof(wifi_config.sta.password) - 1);
	std::memcpy(wifi_config.sta.password, password.data(), password_length);
	wifi_config.sta.password[password_length] = '\0';

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	/* Waiting until either the connection is established (WIFI_CONNECTED_BIT)
	 * or connection failed for the maximum number of re-tries (WIFI_FAIL_BIT).
	 * The bits are set by event_handler() (see above) */
	EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group_,
										   WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
										   pdFALSE, pdFALSE, portMAX_DELAY);
	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", ssid.data(),
				 password.data());
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", ssid.data(),
				 password.data());
	} else {
		ESP_LOGE(TAG, "UNEXPECTED EVENT");
	}
	return ESP_OK;
}

// Handles STA_START (kick off connect), STA_DISCONNECTED (retry up to
// CONFIG_ESP_MAXIMUM_RETRY, then signal WIFI_FAIL_BIT), and
// IP_EVENT_STA_GOT_IP (reset retry count, signal WIFI_CONNECTED_BIT).
void WifiService::sta_event_handler(void *arg, esp_event_base_t event_base,
									int32_t event_id, void *event_data) {
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT &&
			   event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (s_retry_num_ < CONFIG_ESP_MAXIMUM_RETRY) {
			esp_wifi_connect();
			s_retry_num_++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		} else {
			xEventGroupSetBits(s_wifi_event_group_, WIFI_FAIL_BIT);
		}
		ESP_LOGI(TAG, "connect to the AP fail");
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		auto *event = static_cast<ip_event_got_ip_t *>(event_data);
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		s_retry_num_ = 0;
		xEventGroupSetBits(s_wifi_event_group_, WIFI_CONNECTED_BIT);
	}
}

// Disconnects from the currently associated access point.
esp_err_t WifiService::sta_disconnect() { return esp_wifi_disconnect(); }

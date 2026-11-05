//
// Native C counterpart of wifi_service.cpp.
//

#include "wifi_service.h"

#include <esp_log.h>
#include <esp_mac.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <lwip/netdb.h>

#include <string.h>

static const char TAG[] = "wifi_service";

// Copies src into a fixed-size uint8_t field, truncating to fit and always
// leaving a null terminator. Returns the number of bytes copied.
static size_t copy_cstr(uint8_t *dst, size_t dst_size, const char *src) {
	size_t len = strlen(src);
	if (len > dst_size - 1) {
		len = dst_size - 1;
	}
	memcpy(dst, src, len);
	dst[len] = '\0';
	return len;
}

// WIFI_EVENT callback for the AP interface: logs station join/leave.
static void ap_event_handler(wifi_service_t *self, esp_event_base_t event_base,
							 int32_t event_id, void *event_data) {
	(void)self;
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
		wifi_event_ap_staconnected_t *event =
			(wifi_event_ap_staconnected_t *)event_data;
		ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac),
				 event->aid);
	} else if (event_base == WIFI_EVENT &&
			   event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		wifi_event_ap_stadisconnected_t *event =
			(wifi_event_ap_stadisconnected_t *)event_data;
		ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d, reason=%d",
				 MAC2STR(event->mac), event->aid, event->reason);
	}
}

// Handles STA_START (kick off connect), STA_DISCONNECTED (retry up to
// CONFIG_ESP_MAXIMUM_RETRY, then signal WIFI_FAIL_BIT), and
// IP_EVENT_STA_GOT_IP (reset retry count, signal WIFI_CONNECTED_BIT).
static void sta_event_handler(wifi_service_t *self, esp_event_base_t event_base,
							  int32_t event_id, void *event_data) {
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT &&
			   event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (self->retry_num < CONFIG_ESP_MAXIMUM_RETRY) {
			esp_wifi_connect();
			self->retry_num++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		} else {
			xEventGroupSetBits(self->wifi_event_group, WIFI_FAIL_BIT);
		}
		ESP_LOGI(TAG, "connect to the AP fail");
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		self->retry_num = 0;
		xEventGroupSetBits(self->wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

// Single trampoline registered with the event loop; `arg` is the instance.
static void event_handler(void *arg, esp_event_base_t event_base,
						  int32_t event_id, void *event_data) {
	wifi_service_t *self = (wifi_service_t *)arg;
	sta_event_handler(self, event_base, event_id, event_data);
	ap_event_handler(self, event_base, event_id, event_data);
}

esp_err_t wifi_service_init(wifi_service_t *self) {
	memset(self, 0, sizeof(*self));

	self->wifi_event_group = xEventGroupCreate();
	if (self->wifi_event_group == NULL) {
		return ESP_ERR_NO_MEM;
	}

	self->esp_sta = esp_netif_create_default_wifi_sta();
	self->esp_ap = esp_netif_create_default_wifi_ap();

	const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	esp_err_t err = esp_event_handler_instance_register(
		WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, self,
		&self->instance_any_handler_id);
	if (err != ESP_OK) {
		return err;
	}
	return esp_event_handler_instance_register(
		IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, self,
		&self->instance_got_ip_handler_id);
}

void wifi_service_deinit(wifi_service_t *self) {
	esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
										  self->instance_any_handler_id);
	esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP,
										  self->instance_got_ip_handler_id);
	esp_netif_destroy_default_wifi(self->esp_sta);
	esp_netif_destroy_default_wifi(self->esp_ap);
	if (self->wifi_event_group != NULL) {
		vEventGroupDelete(self->wifi_event_group);
		self->wifi_event_group = NULL;
	}
}

esp_err_t wifi_service_ap_connect(wifi_service_t *self) {
	wifi_config_t ap_config = {0};

	ap_config.ap.ssid_len = (uint8_t)copy_cstr(
		ap_config.ap.ssid, sizeof(ap_config.ap.ssid), WIFI_AP_SSID);
	copy_cstr(ap_config.ap.password, sizeof(ap_config.ap.password),
			  WIFI_AP_PASSWORD);
	ap_config.ap.channel = WIFI_AP_CHANNEL;
	ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
	ap_config.ap.ssid_hidden = WIFI_AP_SSID_HIDDEN;
	ap_config.ap.max_connection = WIFI_AP_MAX_CONNECTIONS;
	ap_config.ap.beacon_interval = WIFI_AP_BEACON_INTERVAL;

	// Configure DHCP for the AP
	esp_netif_ip_info_t ap_ip_info = {0};

	esp_netif_dhcps_stop(self->esp_ap); // must call this first
	// Assign access point's static IP, GW, and netmask
	inet_pton(AF_INET, WIFI_AP_IP, &ap_ip_info.ip);
	inet_pton(AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw);
	inet_pton(AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask);
	ESP_ERROR_CHECK(esp_netif_set_ip_info(self->esp_ap, &ap_ip_info));
	// Start the AP DHCP server (for connecting stations, e.g. a phone)
	ESP_ERROR_CHECK(esp_netif_dhcps_start(self->esp_ap));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
	ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_AP_BANDWIDTH));
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE));
	ESP_ERROR_CHECK(esp_wifi_start());

	return ESP_OK;
}

esp_err_t wifi_service_sta_connect(wifi_service_t *self, const char *ssid,
								   const char *password) {
	wifi_config_t wifi_config = {0};
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

	copy_cstr(wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), ssid);
	copy_cstr(wifi_config.sta.password, sizeof(wifi_config.sta.password),
			  password);

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	// Wait until either the connection is established (WIFI_CONNECTED_BIT)
	// or it failed after the maximum number of retries (WIFI_FAIL_BIT).
	EventBits_t bits = xEventGroupWaitBits(self->wifi_event_group,
										   WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
										   pdFALSE, pdFALSE, portMAX_DELAY);
	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "connected to ap SSID:%s", ssid);
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGI(TAG, "Failed to connect to SSID:%s", ssid);
	} else {
		ESP_LOGE(TAG, "UNEXPECTED EVENT");
	}
	return ESP_OK;
}

esp_err_t wifi_service_sta_disconnect(wifi_service_t *self) {
	(void)self;
	return esp_wifi_disconnect();
}

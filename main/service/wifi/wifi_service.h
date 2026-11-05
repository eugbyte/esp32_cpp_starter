//
// Native C counterpart of wifi_service.hpp.
//

#ifndef C_STARTER_WIFI_SERVICE_H
#define C_STARTER_WIFI_SERVICE_H

#include "domain/interface/iwifi_service.h"

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include <esp_err.h>
#include <esp_event.h>
#include <esp_netif_types.h>

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define CONFIG_ESP_MAXIMUM_RETRY 5

#define WIFI_AP_SSID "ESP32_AP"			 // AP name
#define WIFI_AP_PASSWORD "password"		 // AP password
#define WIFI_AP_CHANNEL 1				 // AP channel
#define WIFI_AP_SSID_HIDDEN 0			 // AP visibility
#define WIFI_AP_MAX_CONNECTIONS 5		 // AP max clients
#define WIFI_AP_BEACON_INTERVAL 100		 // AP beacon: 100 ms recommended
#define WIFI_AP_IP "192.168.0.1"		 // AP default IP
#define WIFI_AP_GATEWAY "192.168.0.1"	 // AP gateway (same as the IP)
#define WIFI_AP_NETMASK "255.255.255.0"	 // AP netmask
#define WIFI_AP_BANDWIDTH WIFI_BW20		 // AP bandwidth 20 MHz
#define WIFI_STA_POWER_SAVE WIFI_PS_NONE // Power save not used
#define MAX_SSID_LENGTH 32				 // IEEE standard maximum
#define MAX_PASSWORD_LENGTH 64			 // IEEE standard maximum
#define MAX_CONNECTION_RETRIES 5		 // Retry number on disconnect

/**
 * ESP-IDF Wi-Fi STA+AP service. Uses a FreeRTOS event group plus
 * WIFI_EVENT/IP_EVENT handlers to signal connect success/failure, with
 * automatic reconnect retries up to CONFIG_ESP_MAXIMUM_RETRY. Implements
 * iwifi_service_t; `base` must stay the first member.
 *
 * The instance is registered as the event handler argument, so it must
 * outlive the registration (i.e. until wifi_service_deinit()).
 */
typedef struct wifi_service {
	iwifi_service_t base;
	esp_netif_t *esp_sta;
	esp_netif_t *esp_ap;
	int retry_num;
	EventGroupHandle_t wifi_event_group;
	esp_event_handler_instance_t instance_any_handler_id;
	esp_event_handler_instance_t instance_got_ip_handler_id;
} wifi_service_t;

// Creates STA+AP netifs, inits the Wi-Fi driver (RAM storage), and
// registers the WIFI_EVENT/IP_EVENT handlers.
esp_err_t wifi_service_init(wifi_service_t *self);
// Unregisters the event handlers, destroys the netifs and event group.
void wifi_service_deinit(wifi_service_t *self);
// Starts the software AP alongside STA (WIFI_MODE_APSTA).
esp_err_t wifi_service_ap_connect(wifi_service_t *self);
// Configures STA with the given credentials, starts Wi-Fi, and blocks until
// connected or retries are exhausted.
esp_err_t wifi_service_sta_connect(wifi_service_t *self, const char *ssid,
								   const char *password);
// Disconnects from the currently associated access point.
esp_err_t wifi_service_sta_disconnect(wifi_service_t *self);

// Upcast to the interface.
static inline iwifi_service_t *wifi_service_as_interface(wifi_service_t *self) {
	return &self->base;
}

#endif // C_STARTER_WIFI_SERVICE_H

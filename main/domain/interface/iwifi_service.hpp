//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_STARTER_IWIFISERVICE_H
#define CPP_STARTER_IWIFISERVICE_H
#include <esp_err.h>
#include <etl/string_view.h>

/**
 * @brief Interface for Wi-Fi station (STA) connectivity services.
 *
 * Defines the contract for connecting to and disconnecting from a
 * Wi-Fi access point in station mode.
 */
class IWifiService {
public:
	virtual ~IWifiService() = default;

	/**
	 * @brief Connect to a Wi-Fi access point in station mode.
	 *
	 * Blocks until the connection succeeds or the retry limit is
	 * exhausted.
	 * @param ssid Access point SSID.
	 * @param password Access point password (WPA2/WPA3).
	 * @return ESP_OK once the connection attempt completes; success or
	 *         final failure is reported via logs.
	 */
	virtual esp_err_t sta_connect(etl::string_view ssid,
								  etl::string_view password) = 0;

	/**
	 * @brief Disconnect from the currently associated access point.
	 * @return ESP_OK on success, or an esp_err_t error code on failure.
	 */
	virtual esp_err_t sta_disconnect() = 0;
};

#endif // CPP_STARTER_IWIFISERVICE_H

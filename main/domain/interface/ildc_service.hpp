//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_EXAMPLE_ILCDSERVICE_H
#define CPP_EXAMPLE_ILCDSERVICE_H
#include <esp_err.h>
#include <etl/string_view.h>

/**
 * @brief Interface for LCD display services.
 *
 * Defines the contract for sending text to and clearing a character
 * LCD, independent of the underlying transport (UART, I2C, etc.).
 */
class ILcdService {
public:
	virtual ~ILcdService() = default;

	/**
	 * @brief Write text to the display.
	 * @param text Text to display.
	 * @return ESP_OK on success, or an esp_err_t error code from the
	 *         underlying transport on failure.
	 */
	virtual esp_err_t send_text(etl::string_view text) = 0;

	/**
	 * @brief Clear the display contents.
	 * @return ESP_OK on success, or an esp_err_t error code from the
	 *         underlying transport on failure.
	 */
	virtual esp_err_t clear() = 0;
};

#endif // CPP_EXAMPLE_ILCDSERVICE_H

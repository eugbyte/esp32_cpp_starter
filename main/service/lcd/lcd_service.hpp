//
// Created by eugen on 7/3/2026.
//

#ifndef CPP_EXAMPLE_LCDSERVICE_H
#define CPP_EXAMPLE_LCDSERVICE_H

#include "domain/interface/ildc_service.hpp"
#include <driver/uart.h>
#include <etl/string_view.h>

namespace svc::lcd {

/**
 * @brief UART-based implementation of ILcdService.
 *
 * Drives a serial character LCD over UART1 using vendor-specific
 * control bytes (e.g. 0xFE 0x01 to clear).
 */
class LcdService : public ILcdService {
public:
	LcdService();
	~LcdService() override;
	esp_err_t send_text(etl::string_view text) override;
	esp_err_t clear() override;

private:
	uart_config_t uart_config_{};
};

#define LCD_UART_NUM UART_NUM_1
#define LCD_TX_PIN GPIO_NUM_17
#define LCD_RX_PIN UART_PIN_NO_CHANGE // not used, one-wire
#define LCD_BAUD_RATE 9600			  // common default for serial LCDs

} // namespace svc::lcd

#endif // CPP_EXAMPLE_LCDSERVICE_H

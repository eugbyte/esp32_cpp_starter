//
// Created by eugen on 7/3/2026.
//

#ifndef CPP_EXAMPLE_LCDSERVICE_H
#define CPP_EXAMPLE_LCDSERVICE_H

#include "domain/interface/ILcdService.hpp"
#include "etl/string_view.h"
#include <driver/uart.h>

class LcdService : public ILcdService {
public:
	LcdService();
	~LcdService() override;
	esp_err_t SendText(etl::string_view text) override;
	esp_err_t Clear() override;

private:
	uart_config_t uart_config_{};
};

#define LCD_UART_NUM UART_NUM_1
#define LCD_TX_PIN GPIO_NUM_17
#define LCD_RX_PIN UART_PIN_NO_CHANGE // not used, one-wire
#define LCD_BAUD_RATE 9600			  // common default for serial LCDs

#endif // CPP_EXAMPLE_LCDSERVICE_H

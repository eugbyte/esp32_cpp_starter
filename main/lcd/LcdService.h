//
// Created by eugen on 7/3/2026.
//

#ifndef CPP_EXAMPLE_LCDSERVICE_H
#define CPP_EXAMPLE_LCDSERVICE_H

#include <driver/uart.h>
#include <string_view>

class LcdService {
public:
	LcdService();
	esp_err_t SendText(std::string_view text);
	esp_err_t Clear();

private:
	uart_config_t uart_config;
};

#define LCD_UART_NUM UART_NUM_1
#define LCD_TX_PIN GPIO_NUM_17
#define LCD_RX_PIN UART_PIN_NO_CHANGE // not used, one-wire
#define LCD_BAUD_RATE 9600			  // common default for serial LCDs

#endif // CPP_EXAMPLE_LCDSERVICE_H

//
// Native C counterpart of lcd_service.hpp.
//

#ifndef C_STARTER_LCD_SERVICE_H
#define C_STARTER_LCD_SERVICE_H

#include "domain/interface/ildc_service.h"

#include <driver/gpio.h>
#include <driver/uart.h>

#define LCD_UART_NUM UART_NUM_1
#define LCD_TX_PIN GPIO_NUM_17
#define LCD_RX_PIN UART_PIN_NO_CHANGE // not used, one-wire
#define LCD_BAUD_RATE 9600			  // common default for serial LCDs

/**
 * UART-based LCD service. Drives a serial character LCD over UART1 using
 * vendor-specific control bytes (e.g. 0xFE 0x01 to clear). Implements
 * ilcd_service_t; `base` must stay the first member.
 */
typedef struct lcd_service {
	ilcd_service_t base;
	uart_port_t uart_num;
	uart_config_t uart_config;
} lcd_service_t;

// Configures and installs the UART driver used to talk to the LCD.
esp_err_t lcd_service_init(lcd_service_t *self);
// Uninstalls the UART driver.
void lcd_service_deinit(lcd_service_t *self);
// Writes the raw null-terminated text to the LCD as-is.
esp_err_t lcd_service_send_text(lcd_service_t *self, const char *text);
// Sends the clear-screen command and waits for the LCD to process it.
esp_err_t lcd_service_clear(lcd_service_t *self);

// Upcast to the interface.
static inline ilcd_service_t *lcd_service_as_interface(lcd_service_t *self) {
	return &self->base;
}

#endif // C_STARTER_LCD_SERVICE_H

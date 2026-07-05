//
// Created by eugen on 7/3/2026.
//

#include "lcd_service.hpp"

#include <driver/gpio.h>
#include <driver/uart.h>
#include <esp_log.h>

#include <memory>

using svc::lcd::LcdService;

LcdService::LcdService() {
	uart_config_ = {
		.baud_rate = LCD_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
		.source_clk = UART_SCLK_DEFAULT,
		.flags =
			{
				.allow_pd = 0,
				.backup_before_sleep = 0,
			},
	};

	uart_driver_install(LCD_UART_NUM, 256, 0, 0, nullptr, 0);
	uart_param_config(LCD_UART_NUM, &uart_config_);
	uart_set_pin(LCD_UART_NUM, LCD_TX_PIN, LCD_RX_PIN, UART_PIN_NO_CHANGE,
				 UART_PIN_NO_CHANGE);
}

LcdService::~LcdService() { uart_driver_delete(LCD_UART_NUM); }

esp_err_t LcdService::send_text(etl::string_view text) {
	return uart_write_bytes(LCD_UART_NUM, text.data(), text.length());
}

esp_err_t LcdService::clear() {
	const uint8_t cmd[2] = {0xFE, 0x01};
	esp_err_t err = uart_write_bytes(LCD_UART_NUM, cmd, 2);
	vTaskDelay(pdMS_TO_TICKS(10)); // most LCDs need a short delay after clear
	return err;
}
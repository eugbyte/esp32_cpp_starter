//
// Native C counterpart of lcd_service.cpp.
//

#include "lcd_service.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <string.h>

// Downcast from the interface; valid because `base` is the first member.
#define LCD_SELF(iface) ((lcd_service_t *)(iface))

static esp_err_t vt_send_text(ilcd_service_t *iface, const char *text) {
	return lcd_service_send_text(LCD_SELF(iface), text);
}

static esp_err_t vt_clear(ilcd_service_t *iface) {
	return lcd_service_clear(LCD_SELF(iface));
}

static void vt_deinit(ilcd_service_t *iface) {
	lcd_service_deinit(LCD_SELF(iface));
}

static const ilcd_service_vtable_t LCD_SERVICE_VTABLE = {
	.send_text = vt_send_text,
	.clear = vt_clear,
	.deinit = vt_deinit,
};

esp_err_t lcd_service_init(lcd_service_t *self) {
	self->base.vtable = &LCD_SERVICE_VTABLE;
	self->uart_num = LCD_UART_NUM;
	self->uart_config = (uart_config_t){
		.baud_rate = LCD_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
		.source_clk = UART_SCLK_DEFAULT,
	};

	esp_err_t err = uart_driver_install(self->uart_num, 256, 0, 0, NULL, 0);
	if (err != ESP_OK) {
		return err;
	}
	err = uart_param_config(self->uart_num, &self->uart_config);
	if (err != ESP_OK) {
		return err;
	}
	return uart_set_pin(self->uart_num, LCD_TX_PIN, LCD_RX_PIN,
						UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void lcd_service_deinit(lcd_service_t *self) {
	uart_driver_delete(self->uart_num);
}

esp_err_t lcd_service_send_text(lcd_service_t *self, const char *text) {
	int written = uart_write_bytes(self->uart_num, text, strlen(text));
	return written < 0 ? ESP_FAIL : ESP_OK;
}

esp_err_t lcd_service_clear(lcd_service_t *self) {
	const uint8_t cmd[2] = {0xFE, 0x01};
	int written = uart_write_bytes(self->uart_num, cmd, sizeof(cmd));
	vTaskDelay(pdMS_TO_TICKS(10)); // most LCDs need a short delay after clear
	return written < 0 ? ESP_FAIL : ESP_OK;
}

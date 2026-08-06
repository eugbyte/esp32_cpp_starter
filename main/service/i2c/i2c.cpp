//
// Created by eugen on 8/6/2026.
//

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "i2c.hpp"
#include <stdio.h>
#include <driver/i2c_master.h>
#include <esp_log.h>

using namespace svc::i2c;

I2CService::I2CService() {
	// 1. Configure and create the I2C bus (master)
	i2c_master_bus_config_t bus_config = {};
	bus_config.i2c_port = I2C_MASTER_NUM;
	bus_config.sda_io_num = static_cast<gpio_num_t>(I2C_MASTER_SDA_IO);
	bus_config.scl_io_num = static_cast<gpio_num_t>(I2C_MASTER_SCL_IO);
	bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
	bus_config.glitch_ignore_cnt = 7;
	bus_config.flags.enable_internal_pullup = true;

}
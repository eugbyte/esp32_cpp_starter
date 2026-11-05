//
// Native C counterpart of i2c.cpp.
//

#include "i2c_service.h"

// Downcast from the interface; valid because `base` is the first member.
#define I2C_SELF(iface) ((i2c_service_t *)(iface))
#define I2C_CONST_SELF(iface) ((const i2c_service_t *)(iface))

static esp_err_t vt_subscribe(const ii2c_service_t *iface,
							  uint8_t sensor_address,
							  i2c_master_dev_handle_t *device_handle) {
	return i2c_service_subscribe(I2C_CONST_SELF(iface), sensor_address,
								 device_handle);
}

static esp_err_t vt_unsubscribe(const ii2c_service_t *iface,
								const i2c_master_dev_handle_t *device_handle) {
	return i2c_service_unsubscribe(I2C_CONST_SELF(iface), device_handle);
}

static esp_err_t vt_read(ii2c_service_t *iface,
						 i2c_master_dev_handle_t device_handle,
						 uint8_t reg_addr, uint8_t *data, size_t byte_size) {
	return i2c_service_read(I2C_SELF(iface), device_handle, reg_addr, data,
							byte_size);
}

static esp_err_t vt_write_buffer(ii2c_service_t *iface,
								 i2c_master_dev_handle_t device_handle,
								 const uint8_t *data_buffer_w_reg_addr_prefix,
								 size_t byte_size) {
	return i2c_service_write_buffer(I2C_SELF(iface), device_handle,
									data_buffer_w_reg_addr_prefix, byte_size);
}

static void vt_deinit(ii2c_service_t *iface) {
	i2c_service_deinit(I2C_SELF(iface));
}

static const ii2c_service_vtable_t I2C_SERVICE_VTABLE = {
	.subscribe = vt_subscribe,
	.unsubscribe = vt_unsubscribe,
	.read = vt_read,
	.write_buffer = vt_write_buffer,
	.deinit = vt_deinit,
};

esp_err_t i2c_service_init(i2c_service_t *self) {
	self->base.vtable = &I2C_SERVICE_VTABLE;
	self->bus_handle = NULL;

	// Configure and create the I2C bus (master)
	i2c_master_bus_config_t bus_config = {
		.i2c_port = I2C_MASTER_NUM,
		.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO,
		.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup = true,
	};

	return i2c_new_master_bus(&bus_config, &self->bus_handle);
}

void i2c_service_deinit(i2c_service_t *self) {
	if (self->bus_handle != NULL) {
		i2c_del_master_bus(self->bus_handle);
		self->bus_handle = NULL;
	}
}

esp_err_t i2c_service_subscribe(const i2c_service_t *self,
								uint8_t sensor_address,
								i2c_master_dev_handle_t *device_handle) {
	i2c_device_config_t dev_config = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = sensor_address,
		.scl_speed_hz = I2C_MASTER_FREQ_HZ,
	};
	return i2c_master_bus_add_device(self->bus_handle, &dev_config,
									 device_handle);
}

esp_err_t
i2c_service_unsubscribe(const i2c_service_t *self,
						const i2c_master_dev_handle_t *device_handle) {
	(void)self;
	return i2c_master_bus_rm_device(*device_handle);
}

esp_err_t i2c_service_read(i2c_service_t *self,
						   i2c_master_dev_handle_t device_handle,
						   uint8_t reg_addr, uint8_t *data, size_t byte_size) {
	(void)self;
	return i2c_master_transmit_receive(device_handle, &reg_addr, 1, data,
									   byte_size, I2C_MASTER_TIMEOUT_MS);
}

esp_err_t i2c_service_write_buffer(i2c_service_t *self,
								   i2c_master_dev_handle_t device_handle,
								   const uint8_t *data_buffer_w_reg_addr_prefix,
								   size_t byte_size) {
	(void)self;
	return i2c_master_transmit(device_handle, data_buffer_w_reg_addr_prefix,
							   byte_size, I2C_MASTER_TIMEOUT_MS);
}

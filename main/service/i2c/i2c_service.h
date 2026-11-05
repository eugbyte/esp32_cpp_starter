//
// Native C counterpart of i2c.hpp.
//

#ifndef C_STARTER_I2C_SERVICE_H
#define C_STARTER_I2C_SERVICE_H

#include <driver/i2c_master.h>
#include <esp_err.h>
#include <stddef.h>
#include <stdint.h>

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_TIMEOUT_MS 1000

// I2C master bus service.
typedef struct i2c_service {
	i2c_master_bus_handle_t bus_handle;
} i2c_service_t;

// Configures and creates the I2C master bus (pins, clock source, glitch
// filter, internal pull-ups) from the I2C_MASTER_* macros.
esp_err_t i2c_service_init(i2c_service_t *self);
// Deletes the I2C master bus.
void i2c_service_deinit(i2c_service_t *self);

// Adds a device at sensor_address to the bus and returns its handle via
// device_handle.
esp_err_t i2c_service_subscribe(const i2c_service_t *self,
								uint8_t sensor_address,
								i2c_master_dev_handle_t *device_handle);
// Removes a previously added device from the bus.
esp_err_t i2c_service_unsubscribe(const i2c_service_t *self,
								  const i2c_master_dev_handle_t *device_handle);
// Writes reg_addr then reads byte_size bytes back into data (combined
// write-then-read transaction).
esp_err_t i2c_service_read(i2c_service_t *self,
						   i2c_master_dev_handle_t device_handle,
						   uint8_t reg_addr, uint8_t *data, size_t byte_size);
// Writes byte_size bytes; the first byte is the register address.
esp_err_t i2c_service_write_buffer(i2c_service_t *self,
								   i2c_master_dev_handle_t device_handle,
								   const uint8_t *data_buffer_w_reg_addr_prefix,
								   size_t byte_size);

#endif // C_STARTER_I2C_SERVICE_H

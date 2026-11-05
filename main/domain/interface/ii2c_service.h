//
// Native C counterpart of ii2c_service.hpp.
//
// C "interface": a struct whose only member is a pointer to a vtable of
// function pointers. Concrete services embed ii2c_service_t as their FIRST
// member (`base`) and point it at their own vtable, so a pointer to the
// concrete struct can be passed around as an ii2c_service_t *.
//

#ifndef C_STARTER_II2C_SERVICE_H
#define C_STARTER_II2C_SERVICE_H

#include <driver/i2c_master.h>
#include <esp_err.h>
#include <stddef.h>
#include <stdint.h>

typedef struct ii2c_service ii2c_service_t;

typedef struct ii2c_service_vtable {
	// Registers a device at sensor_address on the I2C bus and returns
	// its handle via device_handle.
	esp_err_t (*subscribe)(const ii2c_service_t *self, uint8_t sensor_address,
						   i2c_master_dev_handle_t *device_handle);
	// Removes a previously subscribed device from the I2C bus.
	esp_err_t (*unsubscribe)(const ii2c_service_t *self,
							 const i2c_master_dev_handle_t *device_handle);
	// Reads byte_size bytes starting at reg_addr from the device into data.
	esp_err_t (*read)(ii2c_service_t *self,
					  i2c_master_dev_handle_t device_handle, uint8_t reg_addr,
					  uint8_t *data, size_t byte_size);
	// Writes byte_size bytes; the first byte is the register address.
	esp_err_t (*write_buffer)(ii2c_service_t *self,
							  i2c_master_dev_handle_t device_handle,
							  const uint8_t *data_buffer_w_reg_addr_prefix,
							  size_t byte_size);
	// Virtual destructor.
	void (*deinit)(ii2c_service_t *self);
} ii2c_service_vtable_t;

struct ii2c_service {
	const ii2c_service_vtable_t *vtable;
};

static inline esp_err_t
ii2c_service_subscribe(const ii2c_service_t *self, uint8_t sensor_address,
					   i2c_master_dev_handle_t *device_handle) {
	return self->vtable->subscribe(self, sensor_address, device_handle);
}

static inline esp_err_t
ii2c_service_unsubscribe(const ii2c_service_t *self,
						 const i2c_master_dev_handle_t *device_handle) {
	return self->vtable->unsubscribe(self, device_handle);
}

static inline esp_err_t ii2c_service_read(ii2c_service_t *self,
										  i2c_master_dev_handle_t device_handle,
										  uint8_t reg_addr, uint8_t *data,
										  size_t byte_size) {
	return self->vtable->read(self, device_handle, reg_addr, data, byte_size);
}

static inline esp_err_t
ii2c_service_write_buffer(ii2c_service_t *self,
						  i2c_master_dev_handle_t device_handle,
						  const uint8_t *data_buffer_w_reg_addr_prefix,
						  size_t byte_size) {
	return self->vtable->write_buffer(self, device_handle,
									  data_buffer_w_reg_addr_prefix, byte_size);
}

static inline void ii2c_service_deinit(ii2c_service_t *self) {
	self->vtable->deinit(self);
}

#endif // C_STARTER_II2C_SERVICE_H

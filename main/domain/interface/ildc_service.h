//
// Native C counterpart of ildc_service.hpp.
//
// Interface for LCD display services, independent of the underlying
// transport (UART, I2C, etc.). See ii2c_service.h for how C interfaces
// (vtable + embedded base) work.
//

#ifndef C_STARTER_ILCD_SERVICE_H
#define C_STARTER_ILCD_SERVICE_H

#include <esp_err.h>

typedef struct ilcd_service ilcd_service_t;

typedef struct ilcd_service_vtable {
	// Writes a null-terminated string to the display.
	esp_err_t (*send_text)(ilcd_service_t *self, const char *text);
	// Clears the display contents.
	esp_err_t (*clear)(ilcd_service_t *self);
	// Virtual destructor.
	void (*deinit)(ilcd_service_t *self);
} ilcd_service_vtable_t;

struct ilcd_service {
	const ilcd_service_vtable_t *vtable;
};

static inline esp_err_t ilcd_service_send_text(ilcd_service_t *self,
											   const char *text) {
	return self->vtable->send_text(self, text);
}

static inline esp_err_t ilcd_service_clear(ilcd_service_t *self) {
	return self->vtable->clear(self);
}

static inline void ilcd_service_deinit(ilcd_service_t *self) {
	self->vtable->deinit(self);
}

#endif // C_STARTER_ILCD_SERVICE_H

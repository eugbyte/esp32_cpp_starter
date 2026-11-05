//
// Native C counterpart of iens160_service.hpp.
//
// See ii2c_service.h for how C interfaces (vtable + embedded base) work.
//

#ifndef C_STARTER_IENS160_SERVICE_H
#define C_STARTER_IENS160_SERVICE_H

#include "domain/model/model.h"

#include <esp_err.h>

typedef struct iens160_service iens160_service_t;

typedef struct iens160_service_vtable {
	// Either pointer may be NULL to skip that compensation value.
	esp_err_t (*connect)(iens160_service_t *self,
						 const float *ambient_temp_celcius_opt,
						 const float *ambient_relative_humidity_opt);
	// Writes the reading into *out_info.
	esp_err_t (*read_air_data)(iens160_service_t *self,
							   ens_160_read_info_t *out_info);
	// Virtual destructor.
	void (*deinit)(iens160_service_t *self);
} iens160_service_vtable_t;

struct iens160_service {
	const iens160_service_vtable_t *vtable;
};

static inline esp_err_t
iens160_service_connect(iens160_service_t *self,
						const float *ambient_temp_celcius_opt,
						const float *ambient_relative_humidity_opt) {
	return self->vtable->connect(self, ambient_temp_celcius_opt,
								 ambient_relative_humidity_opt);
}

static inline esp_err_t
iens160_service_read_air_data(iens160_service_t *self,
							  ens_160_read_info_t *out_info) {
	return self->vtable->read_air_data(self, out_info);
}

static inline void iens160_service_deinit(iens160_service_t *self) {
	self->vtable->deinit(self);
}

#endif // C_STARTER_IENS160_SERVICE_H

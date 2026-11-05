//
// Native C counterpart of ibmp280_service.hpp.
//
// See ii2c_service.h for how C interfaces (vtable + embedded base) work.
//

#ifndef C_STARTER_IBMP280_SERVICE_H
#define C_STARTER_IBMP280_SERVICE_H

#include "domain/model/model.h"

#include <esp_err.h>

typedef struct ibmp280_service ibmp280_service_t;

typedef struct ibmp280_service_vtable {
	esp_err_t (*connect)(ibmp280_service_t *self);
	// Writes the reading into *out_info.
	esp_err_t (*read_thermal)(ibmp280_service_t *self,
							  bmp_280_read_info_t *out_info);
	// Virtual destructor.
	void (*deinit)(ibmp280_service_t *self);
} ibmp280_service_vtable_t;

struct ibmp280_service {
	const ibmp280_service_vtable_t *vtable;
};

static inline esp_err_t ibmp280_service_connect(ibmp280_service_t *self) {
	return self->vtable->connect(self);
}

static inline esp_err_t
ibmp280_service_read_thermal(ibmp280_service_t *self,
							 bmp_280_read_info_t *out_info) {
	return self->vtable->read_thermal(self, out_info);
}

static inline void ibmp280_service_deinit(ibmp280_service_t *self) {
	self->vtable->deinit(self);
}

#endif // C_STARTER_IBMP280_SERVICE_H

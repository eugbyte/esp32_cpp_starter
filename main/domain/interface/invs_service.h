//
// Native C counterpart of invs_service.hpp.
//
// Interface for non-volatile storage (NVS) blob access, namespaced by
// ns_name/key pairs. See ii2c_service.h for how C interfaces (vtable +
// embedded base) work.
//

#ifndef C_STARTER_INVS_SERVICE_H
#define C_STARTER_INVS_SERVICE_H

#include <esp_err.h>
#include <stddef.h>

typedef struct invs_service invs_service_t;

typedef struct invs_service_vtable {
	// Writes the stored blob size into *out_size (0 when the lookup fails).
	esp_err_t (*get_item_size)(invs_service_t *self, const char *ns_name,
							   const char *key, size_t *out_size);
	// Reads required_size bytes of a stored blob into out.
	esp_err_t (*get_blob)(invs_service_t *self, const char *ns_name,
						  const char *key, void *out, size_t required_size);
	// Writes and commits a blob to storage.
	esp_err_t (*write_blob)(invs_service_t *self, const char *ns_name,
							const char *key, const void *data, size_t size);
	// Virtual destructor.
	void (*deinit)(invs_service_t *self);
} invs_service_vtable_t;

struct invs_service {
	const invs_service_vtable_t *vtable;
};

static inline esp_err_t invs_service_get_item_size(invs_service_t *self,
												   const char *ns_name,
												   const char *key,
												   size_t *out_size) {
	return self->vtable->get_item_size(self, ns_name, key, out_size);
}

static inline esp_err_t invs_service_get_blob(invs_service_t *self,
											  const char *ns_name,
											  const char *key, void *out,
											  size_t required_size) {
	return self->vtable->get_blob(self, ns_name, key, out, required_size);
}

static inline esp_err_t invs_service_write_blob(invs_service_t *self,
												const char *ns_name,
												const char *key,
												const void *data, size_t size) {
	return self->vtable->write_blob(self, ns_name, key, data, size);
}

static inline void invs_service_deinit(invs_service_t *self) {
	self->vtable->deinit(self);
}

#endif // C_STARTER_INVS_SERVICE_H

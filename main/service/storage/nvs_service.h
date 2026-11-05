//
// Native C counterpart of nvs_service.hpp.
//

#ifndef C_STARTER_NVS_SERVICE_H
#define C_STARTER_NVS_SERVICE_H

#include "domain/interface/invs_service.h"

#include <esp_err.h>
#include <stddef.h>

// NVS namespace and key names are limited to 15 characters + null
// terminator by the underlying NVS implementation.
#define NVS_SERVICE_MAX_NAME_LEN 16

/**
 * ESP-IDF NVS-backed blob storage. Opens a fresh handle per call scoped to
 * the requested namespace. Implements invs_service_t; `base` must stay the
 * first member.
 */
typedef struct nvs_service {
	invs_service_t base;
} nvs_service_t;

// Binds the vtable. nvs_flash_init() must already have been called.
void nvs_service_init(nvs_service_t *self);
void nvs_service_deinit(nvs_service_t *self);

// Writes key's stored blob size into *out_size (0 on failure).
esp_err_t nvs_service_get_item_size(nvs_service_t *self, const char *ns_name,
									const char *key, size_t *out_size);
// Copies required_size bytes of key's blob into out.
esp_err_t nvs_service_get_blob(nvs_service_t *self, const char *ns_name,
							   const char *key, void *out,
							   size_t required_size);
// Sets key's blob and commits the change.
esp_err_t nvs_service_write_blob(nvs_service_t *self, const char *ns_name,
								 const char *key, const void *data,
								 size_t size);

// Upcast to the interface.
static inline invs_service_t *nvs_service_as_interface(nvs_service_t *self) {
	return &self->base;
}

#endif // C_STARTER_NVS_SERVICE_H

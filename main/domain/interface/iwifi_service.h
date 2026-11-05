//
// Native C counterpart of iwifi_service.hpp.
//
// Interface for Wi-Fi station (STA) connectivity services. See
// ii2c_service.h for how C interfaces (vtable + embedded base) work.
//

#ifndef C_STARTER_IWIFI_SERVICE_H
#define C_STARTER_IWIFI_SERVICE_H

#include <esp_err.h>

typedef struct iwifi_service iwifi_service_t;

typedef struct iwifi_service_vtable {
	// Connects to an access point in station mode; blocks until the
	// connection succeeds or the retry limit is exhausted.
	esp_err_t (*sta_connect)(iwifi_service_t *self, const char *ssid,
							 const char *password);
	// Disconnects from the currently associated access point.
	esp_err_t (*sta_disconnect)(iwifi_service_t *self);
	// Virtual destructor.
	void (*deinit)(iwifi_service_t *self);
} iwifi_service_vtable_t;

struct iwifi_service {
	const iwifi_service_vtable_t *vtable;
};

static inline esp_err_t iwifi_service_sta_connect(iwifi_service_t *self,
												  const char *ssid,
												  const char *password) {
	return self->vtable->sta_connect(self, ssid, password);
}

static inline esp_err_t iwifi_service_sta_disconnect(iwifi_service_t *self) {
	return self->vtable->sta_disconnect(self);
}

static inline void iwifi_service_deinit(iwifi_service_t *self) {
	self->vtable->deinit(self);
}

#endif // C_STARTER_IWIFI_SERVICE_H

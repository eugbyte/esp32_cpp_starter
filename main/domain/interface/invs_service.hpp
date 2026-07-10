//
// Created by eugen on 7/6/2026.
//

#ifndef CPP_STARTER_INVS_SERVICE_H
#define CPP_STARTER_INVS_SERVICE_H

#include "esp_err.h"
#include <cstddef>
#include <etl/span.h>
#include <etl/string_view.h>
#include <etl/tuple.h>


/**
 * @brief Interface for non-volatile storage (NVS) blob access.
 *
 * Defines the contract for reading and writing binary blobs to a
 * key-value store, namespaced by ns_name/key pairs.
 */
class INvsService {
public:
	virtual ~INvsService() = default;

	/**
	 * @brief Get the size in bytes of a stored blob.
	 * @param ns_name Namespace the key lives in.
	 * @param key Key to look up within the namespace.
	 * @return Tuple of (blob size in bytes, esp_err_t status). Size is
	 *         0 when the lookup fails.
	 */
	etl::tuple<size_t, esp_err_t> get_item_size(etl::string_view ns_name,
												etl::string_view key);

	/**
	 * @brief Read a stored blob into a caller-provided buffer.
	 * @param ns_name Namespace the key lives in.
	 * @param key Key to read within the namespace.
	 * @param out Destination buffer; must be at least required_size bytes.
	 * @param required_size Number of bytes to copy into out.
	 * @return ESP_OK on success, or an esp_err_t error code on failure.
	 */
	esp_err_t get_blob(etl::string_view ns_name, etl::string_view key,
					   void *out, size_t required_size);

	/**
	 * @brief Write and commit a blob to storage.
	 * @param ns_name Namespace to write into.
	 * @param key Key to write within the namespace.
	 * @param data Pointer to the source data.
	 * @param size Number of bytes to write.
	 * @return ESP_OK on success, or an esp_err_t error code on failure.
	 */
	esp_err_t write_blob(etl::string_view ns_name, etl::string_view key,
						 void *data, size_t size);
};

#endif // CPP_STARTER_INVS_SERVICE_H

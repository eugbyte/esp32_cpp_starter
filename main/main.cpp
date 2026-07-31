#include "service/httpserver/http_server.hpp"
#include "service/httpserver/web_handler.hpp"
#include "service/lcd/lcd_service.hpp"
#include "service/storage/fs_service.hpp"
#include "service/storage/nvs_service.hpp"
#include "service/wifi/wifi_service.hpp"

#include <etl/span.h>
#include <esp_log.h>
#include <esp_random.h>
#include <etl/string.h>
#include <mqtt_client.h>

#include <cstdio>
#include <esp_netif.h>
#include <nvs_flash.h>

extern "C" void app_main(void) {
	esp_err_t err = {0};

	err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(esp_netif_init());

	printf("Initialized native esp-idf services\n");

	auto lcd_service = svc::lcd::LcdService();
	lcd_service.clear();
	lcd_service.send_text("LCD initialized");

	auto wifi_service = svc::wifi::WifiService();
	etl::string<32> ssid = "Tham Network";
	etl::string<32> password = "28Stratton";
	if (err = wifi_service.sta_connect(ssid, password);
		err != ESP_OK) {
		lcd_service.clear();
		lcd_service.send_text("Wifi connection failed");
	} else {
		lcd_service.clear();
		lcd_service.send_text("Wifi connected");
	}

	wifi_service.ap_connect();

	auto nvs_service = svc::storage::NvsService();
	ESP_LOGI("main", "len: %d, capacity: %d, size: %d", ssid.length(), ssid.capacity(), sizeof(ssid));
	// include null terminator
	// nvs_service.write_blob("wifi", "ssid", ssid.data(), ssid.length() + 1);

	etl::string<32> ssid2("");
	auto [size2, err2] = nvs_service.get_item_size("wifi", "ssid");
	ESP_LOGI("main", "size2: %d", size2);
	etl::span<char> buffer_view(ssid2.data(), size2);
	nvs_service.get_blob("wifi", "ssid", buffer_view.data(), buffer_view.size());

	ESP_LOGI("main", "ssid_from_nvs: |%s|", ssid2.data(), ssid2.length());

	auto web_handler = new svc::httpserver::WebHandler(lcd_service, nvs_service, wifi_service);

	etl::string<32> buffer = "";

	svc::storage::init_fs();
	auto http_server = svc::httpserver::HttpServer();
	using rest_server_context_t = svc::httpserver::rest_server_context_t;

	auto* rest_context = static_cast<rest_server_context_t *>(
		calloc(1, sizeof(rest_server_context_t)));
	etl::string<32> base_path = "/www";
	strlcpy(rest_context->base_path, base_path.data(), sizeof(rest_context->base_path));

	httpd_uri_t common_get_uri = {
		.uri = "/*",
		.method = HTTP_GET,
		.handler = svc::httpserver::WebHandler::serve_static_files,
		.user_ctx = rest_context
	};
	http_server.register_route(&common_get_uri);

	httpd_uri_t healthcheck_uri = {
		.uri = "/healthcheck",
		.method = HTTP_GET,
		.handler = [](httpd_req_t *req) -> esp_err_t {
			const auto wh = static_cast<svc::httpserver::WebHandler *>(req->user_ctx);
			return wh->healthcheck(req);
		},
		.user_ctx = &web_handler
	};
	http_server.register_route(&healthcheck_uri);

	// device reachable at http://esp32.local
	http_server.init_mdns();
	http_server.start_server();

	while (true) {
		const uint32_t random_num = (esp_random() % 10) + 1;
		sprintf(buffer.data(), "num: %d", static_cast<int>(random_num));
		ESP_LOGI("main", "num: %d", random_num);
		lcd_service.clear();
		lcd_service.send_text(buffer.data());

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

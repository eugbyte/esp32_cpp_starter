idf_component_register(
        SRCS
        "main.cpp"
        "service/lcd/lcd_service.cpp"
        "service/wifi/wifi_service.cpp"
        "service/storage/nvs_service.cpp"
        "service/storage/fs_service.cpp"
        "service/httpserver/http_server.cpp" "service/httpserver/web_handler.cpp"
        "service/i2c/i2c.cpp"
        "service/spi/spi.cpp"
        "service/sensor/dht11/dhtsensor_service.cpp"
        "service/sensor/bmp280/bmp280_service.cpp"
        "service/sensor/ens160/ens160_common.cpp"
        "service/sensor/ens160/ens160_i2c_service.cpp"
        "service/sensor/ens160/ens160_spi_service.cpp"
        "task/http_task.cpp"
        "task/wifi_task.cpp"
        "task/i2c_sensor_task.cpp"
        "task/spi_sensor_task.cpp"
        # If adding libraries at git submodules:
        # "${PROJECT_DIR}/submodule_libs/etl/include"
        REQUIRES esp_netif esp_driver_gpio esp_driver_uart esp_wifi nvs_flash esp_http_server http_parser vfs esp_driver_i2c esp_driver_spi
        # to import files use absolute file path
        INCLUDE_DIRS "." "domain/interface" "domain/model"    # DO NOT remove
)

# Link the CPM-fetched library directly to this component target
target_link_libraries(${COMPONENT_LIB} PRIVATE etl::etl)

set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../webpage")
if (EXISTS ${WEB_SRC_DIR}/dist)
    littlefs_create_partition_image(www ${WEB_SRC_DIR}/dist FLASH_IN_PROJECT)
else ()
    message(FATAL_ERROR "'${WEB_SRC_DIR}/dist' doesn't exist. Please run 'pnpm build' under '${WEB_SRC_DIR}'")
endif ()
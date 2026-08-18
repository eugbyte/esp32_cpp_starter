idf_component_register(
        SRCS
        "main.cpp"
        "service/lcd/lcd_service.cpp"
        "service/wifi/wifi_service.cpp"
        "service/storage/nvs_service.cpp"
        "service/storage/fs_service.cpp"
        "service/httpserver/http_server.cpp" "service/httpserver/web_handler.cpp"
        "service/i2c/i2c.cpp"
        "service/sensor/dhtsensor_service.cpp"
        "service/sensor/bmp280_service.cpp"
        "service/sensor/ens160_service.cpp"
        "task/http_task.cpp"
        "task/wifi_task.cpp"
        "task/i2c_sensor_task.cpp"
        # If adding libraries at git submodules:
        # "${PROJECT_DIR}/submodule_libs/etl/include"
        REQUIRES esp_netif esp_driver_gpio esp_driver_uart esp_wifi nvs_flash esp_http_server http_parser vfs esp_driver_i2c
        # to import files use absolute file path
        INCLUDE_DIRS "." "domain/interface" "domain/model"    # DO NOT remove
)

# ETL is header-only and fetched via PlatformIO's Library Manager instead of CPM
# (see `lib_deps` in platformio.ini), so just add its include path to this component.
file(GLOB ETL_INCLUDE_DIR "${PROJECT_DIR}/.pio/libdeps/*/etl/include")
if (NOT ETL_INCLUDE_DIR)
    message(FATAL_ERROR "ETL not found under .pio/libdeps/<env>/etl - run 'pio pkg install' first")
endif ()
target_include_directories(${COMPONENT_LIB} PRIVATE ${ETL_INCLUDE_DIR})

set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../webpage")
if (EXISTS ${WEB_SRC_DIR}/dist)
    littlefs_create_partition_image(www ${WEB_SRC_DIR}/dist FLASH_IN_PROJECT)
else ()
    message(FATAL_ERROR "'${WEB_SRC_DIR}/dist' doesn't exist. Please run 'pnpm build' under '${WEB_SRC_DIR}'")
endif ()
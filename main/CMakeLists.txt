idf_component_register(
        SRCS
        "main.cpp"
        "service/lcd/lcd_service.cpp"
        "service/wifi/wifi_service.cpp"
        "service/storage/nvs_service.cpp"
        "service/httpserver/http_server.cpp" "service/httpserver/web_handler.cpp"
        "service/dhtsensor/dhtsensor_service.cpp"
        INCLUDE_DIRS "."
        # optional, add here public include directories
        # If adding libraries at git submodules:
        # "${PROJECT_DIR}/submodule_libs/etl/include"
        REQUIRES esp_driver_gpio esp_driver_uart esp_wifi nvs_flash esp_http_server http_parser
)

# Link the CPM-fetched library directly to this component target
target_link_libraries(${COMPONENT_LIB} PRIVATE etl::etl)

set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../webpage")
if(EXISTS ${WEB_SRC_DIR}/dist)
    littlefs_create_partition_image(www ${WEB_SRC_DIR}/dist FLASH_IN_PROJECT)
else()
    message(FATAL_ERROR "'${WEB_SRC_DIR}/dist' doesn't exist. Please run 'pnpm build' under '${WEB_SRC_DIR}'")
endif()
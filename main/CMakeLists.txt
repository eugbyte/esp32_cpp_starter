idf_component_register(
        SRCS
        "main.cpp"
        "service/lcd/lcd_service.cpp"
        "service/wifi/wifi_service.cpp"
        INCLUDE_DIRS
        "."    # optional, add here public include directories
        "${PROJECT_DIR}/submodule_libs/etl/include"
        REQUIRES esp_driver_gpio esp_driver_uart esp_wifi nvs_flash
)

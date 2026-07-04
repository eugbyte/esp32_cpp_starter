idf_component_register(
        SRCS
            "main.cpp"
            "lcd/LcdService.cpp"
        INCLUDE_DIRS
            "."    # optional, add here public include directories
            "${PROJECT_DIR}/submodule_libs/etl/include"
        REQUIRES esp_driver_gpio esp_driver_uart
)

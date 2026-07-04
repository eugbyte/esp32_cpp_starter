idf_component_register(
        SRCS
        "main.cpp"
        "lcd/LcdService.cpp"
        "lcd/LcdService.h"
        INCLUDE_DIRS "."    # optional, add here public include directories
        REQUIRES esp_driver_gpio esp_driver_uart
)

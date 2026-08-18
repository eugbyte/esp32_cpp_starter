# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# for libraries downloaded as git submodules under ./submodule_libs
#set(EXTRA_COMPONENT_DIRS "submodule_libs/etl/include")

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)
project(cpp_starter)

# Reapply the mdns include-order fix after managed components are regenerated.
set(MDNS_LWIP_FILE "${PROJECT_DIR}/managed_components/espressif__mdns/mdns_networking_lwip.c")
if (EXISTS "${MDNS_LWIP_FILE}")
    file(READ "${MDNS_LWIP_FILE}" MDNS_LWIP_CONTENT)
    string(FIND "${MDNS_LWIP_CONTENT}" "#include \"esp_netif.h\"" MDNS_HAS_ESP_NETIF_INCLUDE)
    if (MDNS_HAS_ESP_NETIF_INCLUDE EQUAL -1)
        string(REPLACE
                "#include \"esp_log.h\"\n#include \"esp_netif_net_stack.h\""
                "#include \"esp_log.h\"\n#include \"esp_netif.h\"\n#include \"esp_netif_net_stack.h\""
                MDNS_LWIP_CONTENT
                "${MDNS_LWIP_CONTENT}"
        )
        file(WRITE "${MDNS_LWIP_FILE}" "${MDNS_LWIP_CONTENT}")
        message(STATUS "Patched espressif__mdns for ESP-IDF 6.x esp_netif include order")
    endif ()
endif ()

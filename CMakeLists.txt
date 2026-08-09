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

# CPM.cmake bootstrap
set(CPM_DOWNLOAD_VERSION 0.40.2)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if (NOT EXISTS "${CPM_DOWNLOAD_LOCATION}")
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
            "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake"
            "${CPM_DOWNLOAD_LOCATION}"
    )
endif ()
include("${CPM_DOWNLOAD_LOCATION}")


# Add the various libraries you want to use in your project. 
# CPMAddPackage will download the library and make it available for use in your project.
CPMAddPackage(
        NAME etl
        GITHUB_REPOSITORY ETLCPP/etl
        GIT_TAG 20.48.0
)

# Link the libraries to your project. This makes the library available for use in your code. 
# Refer to ./main/CMakeLists.txt for the `target_link_libraries()` usage.
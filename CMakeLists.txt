# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# for libraries downloaded as git submodules under ./submodule_libs
#set(EXTRA_COMPONENT_DIRS "submodule_libs/etl/include")

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)
project(cpp_starter)

# CPM.cmake bootstrap
set(CPM_DOWNLOAD_VERSION 0.40.2)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if(NOT EXISTS "${CPM_DOWNLOAD_LOCATION}")
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
        "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake"
        "${CPM_DOWNLOAD_LOCATION}"
    )
endif()
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
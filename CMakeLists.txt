# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# for libraries downloaded as git submodules under ./submodule_libs
#set(EXTRA_COMPONENT_DIRS "submodule_libs/etl/include")

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)
project(cpp_starter)

# add dependencies
include(cmake/CPM.cmake)

CPMAddPackage(
        NAME etl
        GITHUB_REPOSITORY ETLCPP/etl
        GIT_TAG 20.48.0
)
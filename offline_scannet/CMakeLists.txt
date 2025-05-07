cmake_minimum_required(VERSION 3.22)

project(Offline_Scannet)

option(ILLIXR_INCLUDE_DIR "The root directory for ILLIXR includes")
option(ILLIXR_BUILD_SUFFIX "The build suffix for ILLIXR")
if(NOT ILLIXR_INCLUDE_DIR)
    message(FATAL_ERROR "ILLIXR_INCLUDE_DIR must be given")
elseif(NOT ILLIXR_BUILD_SUFFIX)
    message(FATAL_ERROR "ILLIXR_BUILD_SUFFIX must be given")
endif()

# Set source files for the offline_scannet plugin
set(OFFLINE_SCANNET_SOURCES
    plugin.cpp
    data_loading.hpp
    csv_iterator.hpp
)

# Define the plugin name
set(PLUGIN_NAME plugin.offline_scannet${ILLIXR_BUILD_SUFFIX})

# Create a shared library for the plugin
add_library(${PLUGIN_NAME} SHARED ${OFFLINE_SCANNET_SOURCES})
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PLUGIN_NAME} ${OpenCV_LIBS})
message("${ILLIXR_SOURCE_DIR}")
# Include ILLIXR headers
target_include_directories(${PLUGIN_NAME} PRIVATE ${ILLIXR_INCLUDE_DIR})

# Set required C++ standard
target_compile_features(${PLUGIN_NAME} PRIVATE cxx_std_17)

# Install the plugin
install(TARGETS ${PLUGIN_NAME} DESTINATION lib)

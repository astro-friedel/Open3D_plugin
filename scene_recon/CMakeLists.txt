cmake_minimum_required(VERSION 3.22)
project(Scene_Reconstruction)

option(ILLIXR_INCLUDE_DIR "The root directory for ILLIXR includes")
option(ILLIXR_BUILD_SUFFIX "The build suffix for ILLIXR")
if(NOT ILLIXR_INCLUDE_DIR)
    message(FATAL_ERROR "ILLIXR_INCLUDE_DIR must be given")
elseif(NOT ILLIXR_BUILD_SUFFIX)
    message(FATAL_ERROR "ILLIXR_BUILD_SUFFIX must be given")
endif()

# Required dependencies
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
# find_package(Open3D REQUIRED)
find_package(CUDAToolkit REQUIRED)

# Plugin sources
set(SCENE_RECON_SOURCES
    plugin.cpp
    plugin.hpp
    ${CMAKE_SOURCE_DIR}/include/illixr/data_format/scene_recon_type.hpp
)

# Define plugin name
set(PLUGIN_NAME plugin.scene_recon${ILLIXR_BUILD_SUFFIX})

# Create shared library for the plugin
add_library(${PLUGIN_NAME} SHARED ${SCENE_RECON_SOURCES})

# Include directories
target_include_directories(${PLUGIN_NAME} PRIVATE
    ${ILLIXR_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
)

# C++17 support
target_compile_features(${PLUGIN_NAME} PRIVATE cxx_std_17)

# Link all necessary libraries
target_link_libraries(${PLUGIN_NAME}
    PRIVATE
    # Open3D::Open3D
    CUDA::cudart
    ${OpenCV_LIBS}
)

# Install the plugin
install(TARGETS ${PLUGIN_NAME} DESTINATION lib)
# cmake_minimum_required(VERSION 3.10)
# project(scene_recon_plugin)

# # Required dependencies
# find_package(OpenCV REQUIRED)
# find_package(Eigen3 REQUIRED)
# find_package(CUDAToolkit REQUIRED)
# # find_package(fmt REQUIRED)

# # Plugin sources
# set(SCENE_RECON_SOURCES
#     plugin.cpp
#     plugin.hpp
#     ${CMAKE_SOURCE_DIR}/include/illixr/data_format/scene_recon_type.hpp
# )

# # set (BUILD_OPEN3D_FMT OFF CACHE BOOL "" FORCE)
# # Define plugin name
# set(PLUGIN_NAME plugin.scene_recon${ILLIXR_BUILD_SUFFIX})


# # Create shared library for the plugin
# add_library(${PLUGIN_NAME} SHARED ${SCENE_RECON_SOURCES})

# # Include directories
# target_include_directories(${PLUGIN_NAME} PRIVATE
#     ${ILLIXR_SOURCE_DIR}/include
#     ${OpenCV_INCLUDE_DIRS}
#     ${EIGEN3_INCLUDE_DIR}
#     # Open3D::Open3D
# )

# # C++17 support
# target_compile_features(${PLUGIN_NAME} PRIVATE cxx_std_17)

# # Link all necessary libraries
# target_link_libraries(${PLUGIN_NAME}
#     PRIVATE
#     CUDA::cudart
#     ${OpenCV_LIBS}
# )

# # Install the plugin
# install(TARGETS ${PLUGIN_NAME} DESTINATION lib)

# add_library(plugin SHARED plugin.cpp)

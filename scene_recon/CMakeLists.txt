cmake_minimum_required(VERSION 3.22)
project(Scene_Reconstruction)

option(ILLIXR_INCLUDE_DIR "The root directory for ILLIXR includes")
option(ILLIXR_BUILD_SUFFIX "The build suffix for ILLIXR")
if(NOT ILLIXR_INCLUDE_DIR)
    message(FATAL_ERROR "ILLIXR_INCLUDE_DIR must be given")
elseif(NOT ILLIXR_BUILD_SUFFIX)
    message(FATAL_ERROR "ILLIXR_BUILD_SUFFIX must be given")
endif()
find_program(CLANG_EXE clang)
find_program(CLANG_CXX_EXE NAMES clang-cpp clang++)
if(NOT CLANG_CXX OR NOT CLANG_CXX_EXE)
    message(FATAL_ERROR "This project requires the clang compiler.")
endif()
set(CMAKE_C_COMPILER ${CLANG_EXE})
set(CMAKE_CXX_COMPILER ${CLANG_CXX_EXE})

include(ExternalProject)
externalproject_add(filament
                    GIT_REPOSITORY https://github.com/ILLIXR/filament.git
                    GIT_TAG 5112e4dd5975dfb5001b5b3322d7857361263c39
                    PREFIX ${CMAKE_BINARY_DIR}/_deps/filament
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} _DCMAKE_BUILD_TYPE=Release -DUSE_STATIC_LIBCXX=OFF -DFILAMENT_SKIP_SAMPLES=ON -DFILAMENT_USE_EXTERNAL_GLES3=ON -DFILAMENT_SUPPORTS_WAYLAND=ON -DFILAMENT_SUPPORTS_VULKAN=ON -DFILAMENT_BUILD_FILAMAT=ON -DCMAKE_C_COMPILER=${CLANG_EXE} -DCMAKE_CXX_COMPILER=${CLANG_CXX_EXE}
)

externalproject_add(open3d
                    GIT_REPOSITORY
                    GIT_TAG
                    PREFIX ${CMAKE_BINARY_DIR}/_deps/open3d
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_C_COMPILER=${CLANG_EXE} -DCMAKE_CXX_COMPILER=${CLANG_CXX_EXE}
                    DEPENDS filament
)

# Required dependencies
find_package(OpenCV 4 REQUIRED)
find_package(Eigen3 REQUIRED)
# find_package(Open3D REQUIRED)
find_package(CUDAToolkit REQUIRED)

# Plugin sources
set(SCENE_RECON_SOURCES
    plugin.cpp
    plugin.hpp
    ${ILLIXR_INCLUDE_DIR}/illixr/data_format/scene_recon_type.hpp
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

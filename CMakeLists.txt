cmake_minimum_required(VERSION 3.20)
project(mini_jpeg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

option(MJPEG_ENABLE_CUDA "Build the CUDA backend" OFF)
if(MJPEG_ENABLE_CUDA)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 17)
endif()

add_executable(mjpeg src/main.cpp)

enable_testing()
add_subdirectory(tests)
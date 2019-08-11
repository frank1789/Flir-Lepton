#
cmake_minimum_required(VERSION 3.1)
project(LeptonThermalCamera LANGUAGES C CXX)
set(PROJECT_NAME ThermalCamera)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Build in Release mode")
endif(NOT CMAKE_BUILD_TYPE)

#
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# require package
find_package(Threads)


# add subfolder
#add_subdirectory(leptonSDKEmb32PUB)
#add_subdirectory(thermalcamera)
add_executable(testprog test.cc)
target_compile_options(testprog PUBLIC -fpic -march=native -Ofast -Wall -Werror -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef)

cmake_minimum_required(VERSION 3.7)
# DESCRIPTION "The internal library for breakout board interface and Raspberry Pi GPIO")
project(LeptonSDK)

# LeptonSDK
set(PROJECT_NAME LeptonSDK)
set(LIBRARY_TARGET_NAME ${PROJECT_NAME})
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -fpic -O3)

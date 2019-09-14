# Simple and fast logger for embedded programs C / C ++ interface
cmake_minimum_required(VERSION 3.7)
set(PROJECT_NAME "LOG")
project(${PROJECT_NAME})

# add library LOG
set(PROJECT_NAME ${PROJECT_NAME})
set(LIBRARY_TARGET_NAME ${PROJECT_NAME})
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC -march=native -O2)

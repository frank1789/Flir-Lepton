# Simple and fast logger for embedded programs C / C ++ interface
cmake_minimum_required(VERSION 3.7)
set(LIB_NAME "logger")

# add library looger
set(LIBRARY_TARGET_NAME ${LIB_NAME})
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
)

add_library(${LIB_NAME} ${SOURCES})
target_compile_options(${LIB_NAME} PUBLIC -march=native -fpic -O2)

# Simple and fast logger for embedded programs C / C ++ interface
cmake_minimum_required(VERSION 3.7)
set(LIB_NAME "logger")

# add library looger
set(LIBRARY_TARGET_NAME ${LIB_NAME})
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
)

add_library(${LIB_NAME} "")

target_sources(
    ${LIB_NAME}
    PRIVATE
		${SOURCES}
    PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/logger.h
		${CMAKE_CURRENT_LIST_DIR}/instrumentor.h
)

target_include_directories(
    ${LIB_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_options(${LIB_NAME} PRIVATE -fstack-protector-strong
        -Wunused
        -Wshadow
)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
	if(COVERAGE)
		target_link_libraries(coverage)
	endif()
	else()
	target_compile_options(${LIB_NAME} PUBLIC -fpic -O2)
endif()



cmake_minimum_required (VERSION 3.7)
set(NAME_PROJECT "socket")
project(${PROJECT_NAME})

#-----------------------------------------------------------------------------
# files compilation
#-----------------------------------------------------------------------------
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(${NAME_PROJECT} ${SOURCES})
target_link_libraries(${NAME_PROJECT} 
	Qt5::Widgets 
	Qt5::Network 
	logger)
if (CMAKE_BUILD_TYPE MATCHES "Debug")
	target_compile_options(${NAME_PROJECT} PUBLIC -fpic -O0 -g)
	add_definitions(-D LOGGER_CLIENT)
	if(COVERAGE)
		target_link_libraries(coverage_config)
	endif()
	else()
	target_compile_options(${NAME_PROJECT} PUBLIC -fpic -O2)
endif()
cmake_minimum_required (VERSION 3.7)
set(NAME_PROJECT "socket")
project(${PROJECT_NAME})


#
#
#
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

# compile Debug Mode
add_library(${NAME_PROJECT} ${SOURCES})
target_link_libraries(${NAME_PROJECT} Qt5::Widgets Qt5::Network LOG)
if (CMAKE_BUILD_TYPE MATCHES "Debug")
	message(STATUS "compiling in Debug mode")
	add_definitions(-DLOGGER)
	target_link_libraries(coverage_config)
endif()

# compile Release mode
if( NOT CMAKE_BUILD_TYPE )
	SET( CMAKE_BUILD_TYPE Release ... FORCE )
	message(STATUS "compile in Release mode")
endif()


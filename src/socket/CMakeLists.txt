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
	logger
)

target_compile_options(${NAME_PROJECT} PRIVATE -fpic -fpic -O2)
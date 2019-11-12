#-----------------------------------------------------------------------------
# Minimum cmake version and define project
#-----------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
#set(LEPTONCAMERA "FlirLepton")
#project(${LEPTONCAMERA} LANGUAGES C CXX VERSION "0.0.0")

#-----------------------------------------------------------------------------
# file compilation
#-----------------------------------------------------------------------------
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

# compile Debug Mode
add_executable(${LEPTONCAMERA} ${SOURCES} ${UIS})
target_link_libraries(${LEPTONCAMERA} Qt5::Widgets LeptonSDK LOG ${CMAKE_THREAD_LIBS_INIT} ${raspicam_LIBS})
if (CMAKE_BUILD_TYPE MATCHES "Debug")
	message(STATUS "Compiling debug mode")
	add_definitions(-DLOGGER)
	#target_compile_options(${LEPTONCAMERA} PUBLIC -Wall -Werror -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef)
endif ()
# compile Release mode
if( NOT CMAKE_BUILD_TYPE )
	SET( CMAKE_BUILD_TYPE Release ... FORCE )
	message(STATUS "Active release mode")
#target_compile_options(${LEPTONCAMERA} PUBLIC -march=armv8-a+crc -mtune=cortex-a53 -mfpu=crypto-neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations -O2 -pipe)
endif()



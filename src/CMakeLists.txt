#-----------------------------------------------------------------------------
# Minimum cmake version and define project
#-----------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
#set(LEPTONCAMERA "FlirLepton")
#project(${LEPTONCAMERA} LANGUAGES C CXX VERSION "0.0.0")

#-----------------------------------------------------------------------------
# file compilation
#-----------------------------------------------------------------------------
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/udpsender/*.cpp
)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# compile Debug Mode
add_executable(${LEPTONCAMERA} ${SOURCES} ${UIS})
target_link_libraries(${LEPTONCAMERA}
	Qt5::Widgets
	Qt5::Network
	LeptonSDK
	LOG
	socket
	${CMAKE_THREAD_LIBS_INIT}
	${raspicam_LIBS}
	)
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

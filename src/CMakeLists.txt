#-----------------------------------------------------------------------------
# Minimum cmake version and define project
#-----------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

#-----------------------------------------------------------------------------
# files compilation
#-----------------------------------------------------------------------------
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_executable(${LEPTONCAMERA} ${SOURCES} ${UIS})
target_link_libraries(${LEPTONCAMERA}
	Qt5::Widgets
	Qt5::Network
	LeptonSDK
	logger
	socket
	neuralnetwork
	${CMAKE_THREAD_LIBS_INIT}
	${raspicam_LIBS}
	)

target_compile_options(${LEPTONCAMERA} PUBLIC -fpic -O2)

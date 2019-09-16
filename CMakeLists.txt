CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
set(LEPTONCAMERA "FlirLepton")
project(${LEPTONCAMERA} LANGUAGES C CXX)

# check build type
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
if(APPLE AND EXISTS /usr/local/opt/qt5)
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Find Thread Library
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

# Find raspicam library
list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib")
find_package(raspicam REQUIRED )

# add subdirectory LOG
add_subdirectory(log)
# add subdirectory SDK
add_subdirectory(LeptonSDKEmb32PUB)

# Check architecture
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
message( STATUS "Architecture: ${ARCHITECTURE}" )
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

# compile Debug Mode
add_executable(${LEPTONCAMERA} ${SOURCES})
target_link_libraries(${LEPTONCAMERA} Qt5::Widgets LeptonSDK LOG ${CMAKE_THREAD_LIBS_INIT} ${raspicam_LIBS})
if (CMAKE_BUILD_TYPE MATCHES "Debug")
	message(STATUS "Compiling debug mode")
	add_definitions(-DLOGGER)
        target_compile_options(${LEPTONCAMERA} PUBLIC -march=native -g -O0)
endif ()
# compile Release mode
if( NOT CMAKE_BUILD_TYPE )
	SET( CMAKE_BUILD_TYPE Release ... FORCE )
	message(STATUS "Active release mode")
target_compile_options(${LEPTONCAMERA} PUBLIC -march=native -O2)
endif()



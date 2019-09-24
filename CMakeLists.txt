#-----------------------------------------------------------------------------
# Minimum cmake version and define project
#-----------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
set(LEPTONCAMERA "FlirLepton")
project(${LEPTONCAMERA} LANGUAGES C CXX VERSION "0.0.0")

#-----------------------------------------------------------------------------
# check build type
#-----------------------------------------------------------------------------
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

#-----------------------------------------------------------------------------
# C/C++ standard extensions
#-----------------------------------------------------------------------------
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

#-----------------------------------------------------------------------------
# Git
#-----------------------------------------------------------------------------
# find git program
find_package(Git)

#-----------------------------------------------------------------------------
# Library and Dependecies
#-----------------------------------------------------------------------------
# Find the Qt library
if(APPLE AND EXISTS /usr/local/opt/qt5)
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()
find_package(Qt5 COMPONENTS Widgets REQUIRED)
if (${Qt5_FOUND})
   message(STATUS "Found Qt 5")
else (${Qt5_FOUND})
   message(FATAL_ERROR "Could not locate Qt.")
endif (${Qt5_FOUND})

# Find PThread Library
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

# Find Raspicam library
list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib")
find_package(raspicam REQUIRED)
if (${raspicam_FOUND})
   message (STATUS "Found raspicam")
else (${raspicam_FOUND})
   message (FATAL_ERROR "Could not locate raspicam.")
endif (${raspicam_FOUND})

#-----------------------------------------------------------------------------
# Local library and dependecies
#-----------------------------------------------------------------------------
# add subdirectory LOG
add_subdirectory(log)
# add subdirectory SDK
add_subdirectory(LeptonSDKEmb32PUB)

# Check architecture
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
message( STATUS "Architecture: ${ARCHITECTURE}" )
#-----------------------------------------------------------------------------
# git extract version
#-----------------------------------------------------------------------------
execute_process(COMMAND ${GIT_EXECUTABLE} rev-list --tags --max-count=1 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_REV)
execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags ${GIT_REV} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_VER)

#parse the version information into pieces.
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${GIT_VERSION}")
set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
include_directories(${PROJECT_BINARY_BIN})

# ----------------------------------------------------------------------------
# display status message for important variables
# ----------------------------------------------------------------------------
message( STATUS )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "General configuration for ${PROJECT_NAME} ${PROJECT_VERSION}")
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS )
message("    Compiler:"                   "${CMAKE_COMPILER}"   "${CMAKE_CXX_COMPILER}")
message( STATUS "C flags (Release):       ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
message( STATUS "C flags (Debug):         ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
message( STATUS "C++ flags (Release):       ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
message( STATUS "C++ flags (Debug):         ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
message( STATUS "CMAKE_CXX_FLAGS:         ${CMAKE_CXX_FLAGS}")
message( STATUS "CMAKE_BINARY_DIR:         ${CMAKE_BINARY_DIR}")
message( STATUS )
message( STATUS "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}" )
message( STATUS )
message( STATUS "Change a value with: cmake -D<Variable>=<Value> .." )
message( STATUS )

#-----------------------------------------------------------------------------
# file compilation
#-----------------------------------------------------------------------------
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



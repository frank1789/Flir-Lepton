#-----------------------------------------------------------------------------
# Minimum cmake version and define project
#-----------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
set(LEPTONCAMERA "FlirLepton")
project(${LEPTONCAMERA} LANGUAGES C CXX VERSION "0.6.2")

#-----------------------------------------------------------------------------
# check build type
#-----------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "Compile in debug mode")
	set(CMAKE_BUILD_TYPE Debug)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
	set(CMAKE_BUILD_TYPE Debug)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
	set(CMAKE_BUILD_TYPE Release)
endif()

#-----------------------------------------------------------------------------
# C/C++ standard extensions
#-----------------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#-------------------------------------------------------------------------------
# Select external Library
#-------------------------------------------------------------------------------
# Check architecture
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
set(HEADERS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/tensorflow-lite/include" CACHE INTERNAL "")
include_directories(${HEADERS_PATH})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND "${ARCHITECTURE}" STREQUAL "aarch64")
	set(LIBRARYPATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/tensorflow-lite/aarch64_armv8-a/lib" CACHE INTERNAL "")
    message(STATUS "Detect os: ${CMAKE_SYSTEM_NAME}\tarch: ${ARCHITECTURE}")
    message(STATUS "Set library's directory: ${LIBRARYPATH}")
    link_directories(${LIBRARYPATH})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND "${ARCHITECTURE}" STREQUAL "armv7l")
	set(LIBRARYPATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/tensorflow-lite/rpi_armv7l/lib" CACHE INTERNAL "")
    message(STATUS "Detect os: ${CMAKE_SYSTEM_NAME}\tarch: ${ARCHITECTURE}")
    message(STATUS "Set library's directory: ${LIBRARYPATH}")
    link_directories(${LIBRARYPATH})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND "${ARCHITECTURE}" STREQUAL "x86_64")
    set(LIBRARYPATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/tensorflow-lite/osx_x86_64/lib" CACHE INTERNAL "")
    message(STATUS "Detect os: ${CMAKE_SYSTEM_NAME}\tarch: ${ARCHITECTURE}")
    message(STATUS "Set library's directory: ${LIBRARYPATH}")
    link_directories(${LIBRARYPATH})
endif()

#-----------------------------------------------------------------------------
# Specific Flag for RaspBerry Pi 3 
#-----------------------------------------------------------------------------
set(CMAKE_C_FLAGS_DEBUG "-march=armv8-a+crc -mtune=cortex-a53 -mfpu=crypto-neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations -pipe -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-march=armv8-a+crc -mtune=cortex-a53 -mfpu=crypto-neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations -pipe -pthread")
set(CMAKE_C_FLAGS_RELEASE "-march=armv8-a+crc -mtune=cortex-a53 -mfpu=crypto-neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations -pipe -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "-march=armv8-a+crc -mtune=cortex-a53 -mfpu=crypto-neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations -pipe -pthread")

#-----------------------------------------------------------------------------
# Code coverage
#-----------------------------------------------------------------------------
if(COVERAGE)
	add_library(coverage_config INTERFACE)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	  	message(STATUS "enabled coverage")
	  	# Add required flags (GCC & LLVM/Clang)
	  	target_compile_options(coverage_config INTERFACE
			-O0        # no optimization
			-g         # generate debug info
			-ftest-coverage -fprofile-arcs # sets all required flags
	  	)
	  	if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
			target_link_options(coverage_config INTERFACE --coverage)
	  	else()
			target_link_libraries(coverage_config INTERFACE --coverage)
	  	endif()
	endif()
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

#-----------------------------------------------------------------------------
# Library and Dependecies
#-----------------------------------------------------------------------------
# Find the Qt library
if(APPLE AND EXISTS /usr/local/opt/qt5)
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
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
add_subdirectory(src)
add_subdirectory(src/socket)
add_subdirectory(src/log)
add_subdirectory(src/LeptonSDKEmb32PUB)
add_subdirectory(tf)

# wrap ui
file(GLOB UIS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)
# Check architecture
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

# ----------------------------------------------------------------------------
# display status message for important variables
# ----------------------------------------------------------------------------
message( STATUS )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "General configuration for ${PROJECT_NAME} ${PROJECT_VERSION}")
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS )
message( STATUS "Architecture:\t${ARCHITECTURE}" )
message( STATUS "Compiler:\t${CMAKE_COMPILER}\t${CMAKE_CXX_COMPILER}")
message( STATUS "C flags (Release): \t${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
message( STATUS "C flags (Debug): \t${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
message( STATUS "C++ flags (Release):\t${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
message( STATUS "C++ flags (Debug):\t${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
message( STATUS "CMAKE_BINARY_DIR:\t${CMAKE_BINARY_DIR}")
message( STATUS )
message( STATUS "Change a value with: cmake -D<Variable>=<Value> .." )
message( STATUS )

# ----------------------------------------------------------------------------
# copy resources
# ----------------------------------------------------------------------------
file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src)

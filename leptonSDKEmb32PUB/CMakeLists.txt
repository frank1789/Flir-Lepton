cmake_minimum_required(VERSION 3.7)
project(LibLeptonSDK)# DESCRIPTION "The internal library for breakout board interface and Raspberry Pi GPIO")

# LeptonSDK
set(PROJECT_NAME LeptonSDK)
set(LIBRARY_TARGET_NAME ${PROJECT_NAME})
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# Lists of sources and headers
set(${LIBRARY_TARGET_NAME}_SRC ${SOURCES})
set(${LIBRARY_TARGET_NAME}_HDR ${HEADERS})
add_library(${LIBRARY_TARGET_NAME} SHARED ${${LIBRARY_TARGET_NAME}_SRC} ${${LIBRARY_TARGET_NAME}_HDR})
target_include_directories(${LIBRARY_TARGET_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_options(${LIBRARY_TARGET_NAME} PUBLIC
    -fpic
    -O2
)

install(TARGETS
        ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Config
    ARCHIVE DESTINATION lib/static
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
export(TARGETS
    ${PROJECT_NAME}
    NAMESPACE ${PROJECT_NAME}::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
)
install(EXPORT
    ${PROJECT_NAME}Config
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake"
    NAMESPACE ${PROJECT_NAME}::
)

message(STATUS "Created target ${LIBRARY_TARGET_NAME} for export ${PROJECT_NAME}.")

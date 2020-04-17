cmake_minimum_required(VERSION 3.16)
set(LIB_NAME "neuralnetwork")

#-------------------------------------------------------------------------------
# source neural network
#-------------------------------------------------------------------------------
set(LIBRARY_TARGET_NAME ${LIB_NAME})
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/list_colour.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/colormanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/findmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model_tpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/label_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/labels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/object_detection.cpp
)

set(HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/list_colour.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/colormanager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/findmodel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model_tpu.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/label_utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/labels.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tensordata_impl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tensordata.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/image_utils_impl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/image_utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/object_detection.hpp
)

#-------------------------------------------------------------------------------
# neural network library
#-------------------------------------------------------------------------------
add_library(${LIB_NAME} "")
target_link_libraries(${LIB_NAME}
    Qt5::Widgets 
    Qt5::Network 
    logger 
    -ltensorflow-lite
)

target_sources(${LIB_NAME}
    PUBLIC ${SOURCES} ${HEADERS}
)

target_include_directories(${LIB_NAME} 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_options(${LIB_NAME} PRIVATE -fstack-protector-strong
        -Wunused
        -Wshadow
)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
	if(COVERAGE)
		target_link_libraries(coverage)
	endif()
	else()
	target_compile_options(${NAME_PROJECT} PUBLIC -fpic -O2)
endif()

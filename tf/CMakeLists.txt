# Simple and fast logger for embedded programs C / C ++ interface
cmake_minimum_required(VERSION 3.7)
set(LIB_NAME "neuralnetwork")

#-------------------------------------------------------------------------------
# add library neural network
#-------------------------------------------------------------------------------
set(LIBRARY_TARGET_NAME ${LIB_NAME})
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/list_colour.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/colormanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/findmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model_tpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/util_label_image.cpp
)

add_library(${LIB_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${LIB_NAME}
    Qt5::Widgets 
    Qt5::Network 
    logger 
    -ltensorflow-lite
)
target_compile_options(${LIB_NAME} PUBLIC -fpic -O2)

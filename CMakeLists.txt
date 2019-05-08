cmake_minimum_required(VERSION 3.12)
project(PiCamera)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_GLOBALLY "Install it globally to system" OFF)

set(MODEL_DIR ${PROJECT_SOURCE_DIR}/models)

if(BUILD_GLOBALLY)
    set(MODEL_DIR ${CMAKE_INSTALL_PREFIX}/share/PiCamera)
endif(BUILD_GLOBALLY)

set(MODEL_PROTO deploy.prototxt)
set(MODEL_BINARY res10_300x300_ssd_iter_140000_fp16.caffemodel)

configure_file("${PROJECT_SOURCE_DIR}/PiCamera.h.in"
               "${PROJECT_SOURCE_DIR}/sources/picamera/PiCamera.h")

message(${PROJECT_BINARY_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/libs)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

set(VIDEO_HEADER_DIR ${PROJECT_SOURCE_DIR}/sources/video/include)
set(PICAMERA_HEADER_DIR ${PROJECT_SOURCE_DIR}/sources/picamera/include)

find_package(FFmpeg REQUIRED)
find_package(OpenCV 4 REQUIRED)
find_package (Threads REQUIRED)
#
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_subdirectory(sources/video)
add_subdirectory(sources/picamera)

add_dependencies(PiCamera video)

install(TARGETS PiCamera DESTINATION bin)
install(FILES models/${MODEL_PROTO} models/${MODEL_BINARY} DESTINATION ${MODEL_DIR})
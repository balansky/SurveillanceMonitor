cmake_minimum_required(VERSION 3.12)
project(PiCamera)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCE "src/*.cpp")

option(BUILD_GLOBALLY "Install it globally to system" OFF)

set(MODEL_DIR ${PROJECT_SOURCE_DIR}/models)

message(${BUILD_GLOBALLY})

if(BUILD_GLOBALLY)
    set(MODEL_DIR ${CMAKE_INSTALL_PREFIX}/share/PiCamera)
endif(BUILD_GLOBALLY)

message(${MODEL_DIR})

set(MODEL_PROTO deploy.prototxt)
set(MODEL_BINARY res10_300x300_ssd_iter_140000_fp16.caffemodel)

configure_file("${PROJECT_SOURCE_DIR}/PiCamera.h.in" 
               "${PROJECT_INCLUDE_DIR}/PiCamera.h")
            
find_package(OpenCV 4 REQUIRED)
find_package(FFmpeg REQUIRED)
find_package (Threads REQUIRED)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

message(${FFMPEG_LIBRARIES})

include_directories(${PROJECT_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS} ${FFMPEG_INCLUDE_DIRS})

add_executable(PiCamera src/main.cpp ${SOURCE} )

target_link_libraries(PiCamera ${OpenCV_LIBS} ${FFMPEG_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS PiCamera DESTINATION bin)
install(FILES models/${MODEL_PROTO} models/${MODEL_BINARY} DESTINATION ${MODEL_DIR})
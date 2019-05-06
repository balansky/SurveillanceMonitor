


find_package(OpenCV 4 REQUIRED)

set(VIDEO_HEADER_DIR ${PROJECT_SOURCE_DIR}/sources/video/include)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

include_directories("include" ${OpenCV_INCLUDE_DIRS} ${VIDEO_HEADER_DIR})

add_executable(PiCamera main.cpp PiCamera.h)

#target_link_libraries(PiCamera libvideo.a)
target_link_libraries(PiCamera ${PROJECT_SOURCE_DIR}/cmake-build-debug/sources/video/libvideo.a)

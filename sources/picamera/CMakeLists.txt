
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

include_directories(${PICAMERA_HEADER_DIR} ${OpenCV_INCLUDE_DIRS} ${VIDEO_HEADER_DIR})


file(GLOB_RECURSE PICAMERA_SOURCE "src/*.cpp")

add_executable(PiCamera main.cpp PiCamera.h ${PICAMERA_SOURCE})

#target_link_libraries(PiCamera libvideo.a)
target_link_libraries(PiCamera libvideo.so ${OpenCV_LIBS} ${FFMPEG_LIBRARIES})

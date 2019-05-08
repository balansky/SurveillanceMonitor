

file(GLOB_RECURSE VIDEO_SOURCE "src/*.cpp")

include_directories("include" ${FFMPEG_INCLUDE_DIRS})

add_library(video SHARED ${VIDEO_SOURCE})

target_link_libraries(video ${FFMPEG_LIBRARIES})

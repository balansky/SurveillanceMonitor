

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

find_package(FFmpeg REQUIRED)

file(GLOB_RECURSE VIDEO_SOURCE "src/*.cpp")

include_directories("include")

add_library(video STATIC ${VIDEO_SOURCE} ${VIDEO_HEADERS})

target_link_libraries(video ${FFMPEG_LIBRARIES})

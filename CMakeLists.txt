cmake_minimum_required(VERSION 3.12)
project(PiCamera)

set(CMAKE_CXX_STANDARD 14)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# file(GLOB_RECURSE SOURCE "src/*.cpp")

find_package(OpenCV 4 REQUIRED)

# message(${OpenCV_INCLUDE_DIRS})

include_directories(${PROJECT_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})

# add_executable(PiCamera src/main.cpp ${SOURCE})
add_executable(PiCamera src/main.cpp src/surveillance.cpp)

target_link_libraries(PiCamera ${OpenCV_LIBS})
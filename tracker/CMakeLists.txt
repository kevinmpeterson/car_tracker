cmake_minimum_required(VERSION 2.8)

project(TRACKER)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

include_directories(include)

add_subdirectory(src)
add_subdirectory(test)

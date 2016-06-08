cmake_minimum_required(VERSION 3.5)
project(JZXInterface)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(../com/)
set(SOURCE_FILES main.cpp)
add_library(JZXInterface SHARED ${SOURCE_FILES})
target_link_libraries(JZXInterface)
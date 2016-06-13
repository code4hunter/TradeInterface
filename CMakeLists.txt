cmake_minimum_required(VERSION 3.5)
project(TradeInterface)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES com/time_utlity.cpp com/time_utlity.h)
add_library(TradeInterface STATIC ${SOURCE_FILES})

find_path(Boost_cfg NAMES project-config.jam PATHS F:\\lib\\boost_1_61_0 D:\\lib\\boost_1_61_0)
set(Boost_INCLUDE_DIR ${Boost_cfg})

find_package(Boost COMPONENTS system thread log_setup log date_time filesystem chrono)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()

target_link_libraries(TradeInterface ${Boost_LIBRARIES})

ADD_SUBDIRECTORY(JZXInterface)

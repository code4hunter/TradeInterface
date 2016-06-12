cmake_minimum_required(VERSION 3.5)
project(JZXInterface)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES main.cpp ImpTradeSession.cpp ImpTradeSession.h ../com/TradeAPI.h encode_dll_wrapper.cpp)
add_library(JZXInterface SHARED ${SOURCE_FILES})

set(Boost_INCLUDE_DIR F:\\lib\\boost_1_61_0)

find_package(Boost COMPONENTS system thread log_setup log date_time filesystem chrono)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()

add_library(KCBPCli SHARED IMPORTED)
set_property(TARGET KCBPCli PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/KCBPCli.dll)
set_property(TARGET KCBPCli PROPERTY IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/lib/KCBPCli.lib)
target_link_libraries(JZXInterface KCBPCli ${Boost_LIBRARIES})
add_executable(testJZXInterface ${SOURCE_FILES})
target_link_libraries(testJZXInterface KCBPCli ${Boost_LIBRARIES})
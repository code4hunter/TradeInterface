cmake_minimum_required(VERSION 3.5)
project(JZXInterface)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp ImpTradeSession.cpp ImpTradeSession.h encode_dll_wrapper.cpp ../com/time_utlity.cpp
        ../com/record_set.cpp ../com/record_set.h ../com/utlity.cpp ../com/utlity.h)
add_library(JZXInterface SHARED ${SOURCE_FILES})

find_path(Boost_cfg NAMES project-config.jam PATHS F:\\lib\\boost_1_61_0 D:\\lib\\boost_1_61_0)
set(Boost_INCLUDE_DIR ${Boost_cfg})

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

add_executable(testdll testdll.cpp)
target_link_libraries(testdll JZXInterface)
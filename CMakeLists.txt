cmake_minimum_required(VERSION 2.6)
project(umilog)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC")

add_executable(umilog main.cpp)

target_link_libraries(umilog boost_system pthread ssl crypto gtest gtest_main)


install(TARGETS umilog RUNTIME DESTINATION bin)


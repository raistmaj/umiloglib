cmake_minimum_required(VERSION 2.6)
project(umilog)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC")

add_library(umilog SHARED umilog.cpp
        umilog.hpp
        umilogtimestamp.hpp
        umilocaldata.hpp
        umilogconnection.hpp
        umisocket_priv.hpp
        umifacility.hpp
        umiseverity.hpp)

install(TARGETS umilog
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION bin
        LIBRARY DESTINATION bin)


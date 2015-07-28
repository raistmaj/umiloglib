cmake_minimum_required(VERSION 2.6)
project(umilog)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")

add_library(umilog SHARED umilog.cpp
  umilog.hpp
  umilogtimestamp.cpp
  umilogtimestamp.hpp
  umilocaldata.cpp
  umilocaldata.hpp
  umilogconnection.cpp
  umilogconnection.hpp
  umisocket.cpp
  umisocket.hpp
  umifacility.hpp
  umiseverity.hpp
  umiseverity.cpp)

install(TARGETS umilog
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION bin
  LIBRARY DESTINATION bin)


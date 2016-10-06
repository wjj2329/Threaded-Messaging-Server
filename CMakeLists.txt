cmake_minimum_required(VERSION 3.6)
project(cs360project2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    client.cc
    client.h
    ClientManager.h
    echo-client.cc
    echo-server.cc
    message.cc
    message.h
    server.cc
    server.h)

add_executable(cs360project2 ${SOURCE_FILES})
cmake_minimum_required(VERSION 3.10)
project(cpp_server)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/inc)

add_executable(server server.cpp)

add_executable(client client.cpp)
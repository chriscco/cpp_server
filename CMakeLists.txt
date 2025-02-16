cmake_minimum_required(VERSION 3.10)
project(cpp_server)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/inc)

add_executable(cpp_server server.cpp)

add_executable(cpp_client client.cpp)
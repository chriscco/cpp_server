cmake_minimum_required(VERSION 3.10)
project(cpp_server)

set(CMAKE_CXX_STANDARD 11)

add_executable(cpp_thread_pool server.cpp
                client.cpp)
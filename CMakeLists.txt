cmake_minimum_required(VERSION 3.10)
project(cpp_server)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/inc)

set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/socket.cpp
    ${PROJECT_SOURCE_DIR}/src/channel.cpp
    ${PROJECT_SOURCE_DIR}/src/epoll.cpp
    ${PROJECT_SOURCE_DIR}/src/inetaddr.cpp
    ${PROJECT_SOURCE_DIR}/src/error_handler.cpp
    ${PROJECT_SOURCE_DIR}/src/event_loop.cpp
    ${PROJECT_SOURCE_DIR}/src/server.cpp
    ${PROJECT_SOURCE_DIR}/src/acceptor.cpp
    ${PROJECT_SOURCE_DIR}/src/connection.cpp
    ${PROJECT_SOURCE_DIR}/src/buffer.cpp
    ${PROJECT_SOURCE_DIR}/src/threadpool.cpp
)
add_executable(driver driver.cpp ${SRC_FILES})

add_executable(client client.cpp ${SRC_FILES})

target_link_libraries(driver pthread)

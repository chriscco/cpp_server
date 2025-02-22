cmake_minimum_required(VERSION 3.10)
project(cpp_server)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/inc)

find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/channel.cpp
    ${PROJECT_SOURCE_DIR}/src/epoll.cpp
    ${PROJECT_SOURCE_DIR}/src/inetaddr.cpp
    ${PROJECT_SOURCE_DIR}/src/error_handler.cpp
    ${PROJECT_SOURCE_DIR}/src/event_loop.cpp
    ${PROJECT_SOURCE_DIR}/src/tcpserver.cpp
    ${PROJECT_SOURCE_DIR}/src/acceptor.cpp
    ${PROJECT_SOURCE_DIR}/src/tcpconnection.cpp
    ${PROJECT_SOURCE_DIR}/src/buffer.cpp
    ${PROJECT_SOURCE_DIR}/src/threadpool.cpp
    ${PROJECT_SOURCE_DIR}/src/currentThread.cpp
)
add_executable(driver driver.cpp ${SRC_FILES})

# add_executable(client client.cpp ${SRC_FILES})

# add_executable(test test.cpp ${SRC_FILES})

target_link_libraries(driver PRIVATE Threads::Threads)

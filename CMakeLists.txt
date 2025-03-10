cmake_minimum_required(VERSION 3.10)
project(cpp_server)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/inc)

include_directories(/usr/include) # numa.h
link_directories(/usr/lib/x86_64-linux-gnu) # libnuma.so
 
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lnuma")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -lnuma")

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
    ${PROJECT_SOURCE_DIR}/src/eventLoopThread.cpp
    ${PROJECT_SOURCE_DIR}/src/eventLoopThreadPool.cpp
    ${PROJECT_SOURCE_DIR}/src/timerQueue.cpp
    ${PROJECT_SOURCE_DIR}/src/timer.cpp
    ${PROJECT_SOURCE_DIR}/src/logging.cpp
    ${PROJECT_SOURCE_DIR}/src/logFile.cpp
    ${PROJECT_SOURCE_DIR}/src/logStream.cpp
)
add_executable(driver driver.cpp ${SRC_FILES})

# add_executable(test logTest.cpp ${SRC_FILES})

add_executable(client client.cpp ${SRC_FILES})

target_link_libraries(driver PRIVATE Threads::Threads numa)
target_link_libraries(client PRIVATE Threads::Threads numa)

#pragma once 
#include "common.h"
#include "event_loop.h"
class EventLoop;

class EventLoopThread {
    public:
        DISALLOW_COPY_MOVE(EventLoopThread);
        EventLoopThread();
        ~EventLoopThread();

        // 启动线程， 使EventLoop成为IO线程
        EventLoop *startLoop();

    private:
        // 线程运行的函数
        void threadFunction();

        // 指向由子线程创建的的EventLoop，用于主线程和子线程的数据传递
        EventLoop *_loop;
        std::thread _thread;
        std::mutex _mutex;
        std::condition_variable _condition_var;
};
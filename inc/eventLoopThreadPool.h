#pragma once
#include "common.h"
#include "event_loop.h"
#include "eventLoopThread.h"

class EventLoopThreadPool{

    public:
        DISALLOW_COPY_MOVE(EventLoopThreadPool);
        EventLoopThreadPool(EventLoop *loop);
        ~EventLoopThreadPool();

        void setThreadSize(int thread_nums);

        void start();

        // 获取线程池中的EventLoop
        EventLoop *nextloop();

    private:
        EventLoop *_mainReactor;
        std::vector<std::unique_ptr<EventLoopThread>> _threads;

        std::vector<EventLoop *> _loops;

        int _thread_num;

        int _next;
};
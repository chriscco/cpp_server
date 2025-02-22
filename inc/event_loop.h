#pragma once
#include <vector>
#include <sys/eventfd.h>
#include <cassert>
#include <thread>

#include "../inc/threadpool.h"
#include "../inc/epoll.h"
#include "../inc/common.h"
#include "../inc/currentThread.h"
class Epoll;
class Channel;
class Server; 
class ThreadPool;

class EventLoop {
private:
    std::unique_ptr<Epoll> _epoll;
    std::vector<std::function<void()>> _todoList;
    pid_t _thread_id;
    std::mutex _mutex;
    int _fd;
    bool _callingFunc;
    std::unique_ptr<Channel> _channel;

public:
    DISALLOW_COPY_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();

    void updateChannel(Channel*) const;
    void deleteChannel(Channel*) const;
    void loop();
    void handleRead();

    /** 判断当前线程是否是主线程 */
    bool ifInMainThread();
    void runFunction(std::function<void()> func);
    void queueFunction(std::function<void()> func);

    void runList();
};
#pragma once
#include <vector>

#include "../inc/threadpool.h"
#include "../inc/epoll.h"
#include "../inc/common.h"

class Epoll;
class Channel;
class Server; 
class ThreadPool;

class EventLoop {
private:
    std::unique_ptr<Epoll> _epoll;

public:
    DISALLOW_COPY_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();

    void updateChannel(Channel*) const;
    void deleteChannel(Channel*) const;
    void loop();
};
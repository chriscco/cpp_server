#pragma once
#include <vector>

#include "../inc/threadpool.h"
#include "../inc/epoll.h"
class Epoll;
class Channel;
class Server; 
class ThreadPool;

class EventLoop {
private:
    Epoll* _epoll;
    bool _quit;

public:
    EventLoop();
    ~EventLoop();

    void updateChannel(Channel*);
    void loop();
};
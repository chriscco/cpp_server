#pragma once
#include <vector>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>

#include "channel.h"
#include "error_handler.hpp"

#define MAX_EVENTS_SIZE 1024

class Channel; 

class Epoll {
private:
    int _epoll_fd;
    struct epoll_event* _events;
public:
    Epoll();
    ~Epoll();

    void add_fd(int fd, uint32_t op);

    std::vector<Channel*> poll_events(int timeout = -1);

    void updateChannel(Channel*);
};
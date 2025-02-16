#pragma once
#include <vector>
#include <sys/epoll.h>
#include "inc/error_handler.hpp"
#include <stdlib.h>
#include <string.h>

#define MAX_EVENTS_SIZE 1024

class Epoll {
private:
    int _epoll_fd;
    struct epoll_event* _events;
public:
    Epoll() {};
    ~Epoll() {};

    void add_fd(int fd, uint32_t op);

    std::vector<epoll_event> add_event(int timeout);
};

Epoll::Epoll() : _epoll_fd(-1), _events(nullptr) {
    _epoll_fd = epoll_create1(0);
    errif(_epoll_fd < 0, "epoll_fd creation error.");
    _events = new epoll_event[MAX_EVENTS_SIZE];
    bzero(_events, sizeof(*_events) * MAX_EVENTS_SIZE);
}

/**
 * @param fd
 * @param op 代表了可读/可写/错误事件, e.g. EPOLLIN/EPOLLOUT
 */
void Epoll::add_fd(int fd, uint32_t op) {
    struct epoll_event evt;
    bzero(&evt, sizeof(evt));
    evt.data.fd = fd;
    evt.events = op;
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &evt);
}

std::vector<epoll_event> Epoll::add_event(int timeout) {
    std::vector<epoll_event> eventsRetrieved;
    int nfds = epoll_wait(_epoll_fd, _events, MAX_EVENTS_SIZE, timeout);
    errif(nfds, "epoll wait error.");
    for (int i = 0; i < nfds; ++i) {
        eventsRetrieved.emplace_back(_events[i]);
    }
    return eventsRetrieved;
}
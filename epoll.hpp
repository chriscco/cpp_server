#pragma once
#include <vector>
#include <sys/epoll.h>
#include "inc/error_handler.hpp"
#include <stdlib.h>
#include <string.h>

#define MAX_EVENTS_SIZE 1024

class Epoll {
private:
    int epoll_fd;
    struct epoll_event* events;
public:
    Epoll() {};
    ~Epoll() {};

    void add_fd(int fd, uint32_t op);

    std::vector<epoll_event> add_event(int timeout);
};

Epoll::Epoll() : epoll_fd(-1), events(nullptr) {
    epoll_fd = epoll_create1(0);
    errif(epoll_fd < 0, "epoll_fd creation error.");
    events = new epoll_event[MAX_EVENTS_SIZE];
    bzero(events, sizeof(*events) * MAX_EVENTS_SIZE);
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
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &evt);
}

std::vector<epoll_event> Epoll::add_event(int timeout) {
    std::vector<epoll_event> eventsRetrieved;
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS_SIZE, timeout);
    errif(nfds, "epoll wait error.");
    for (int i = 0; i < nfds; ++i) {
        eventsRetrieved.emplace_back(events[i]);
    }
    return eventsRetrieved;
}
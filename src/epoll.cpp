#include "../inc/epoll.h"

Epoll::Epoll() : _epoll_fd(-1), _events(nullptr) {
    _epoll_fd = epoll_create1(0);
    errif(_epoll_fd < 0, "epoll_fd creation error.");
    _events = new epoll_event[MAX_EVENTS_SIZE];
    bzero(_events, sizeof(*_events) * MAX_EVENTS_SIZE);
}

Epoll::~Epoll() {
    if (_epoll_fd != -1) {
        close(_epoll_fd);
        _epoll_fd = -1;
    }
    delete [] _events;
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

std::vector<Channel*> Epoll::poll_events(int timeout) {
    std::vector<Channel*> eventsRetrieved;
    int nfds = epoll_wait(_epoll_fd, _events, MAX_EVENTS_SIZE, timeout);
    errif(nfds < 0, "epoll wait error.");
    for (int i = 0; i < nfds; i++) {
        // 此处存储的在 updateChannel 中存储到data.ptr的Channel对象指针
        Channel* ch = (Channel*)_events[i].data.ptr;
        ch->setrevent(_events[i].events);
        eventsRetrieved.emplace_back(ch);
    }
    return eventsRetrieved;
}

void Epoll::updateChannel(Channel* ch) {
    int fd = ch->getfd();
    struct epoll_event evt;
    bzero(&evt, sizeof(evt));
    evt.data.ptr = ch;
    evt.events = ch->getevent();
    if (ch->getRegisterFlag()) {
        errif(epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &evt) == -1, "epoll mod error.");
    } else {
        errif(epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &evt) == -1, "epoll add error.");
        ch->setRegisterFlag();
    }
}
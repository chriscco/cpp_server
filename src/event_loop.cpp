#include "../inc/event_loop.h"

EventLoop::EventLoop() : _epoll(nullptr), _quit(false) {
    _epoll = new Epoll();
}
EventLoop::~EventLoop() {
    delete _epoll;
}

void EventLoop::loop() {
    while (!_quit) {
        std::vector<Channel*> channels = _epoll->poll_events();
        for (auto& ch : channels) {
            ch->handleEvent();
        }
    }
}
void EventLoop::updateChannel(Channel* ch) {
    _epoll->updateChannel(ch);
}

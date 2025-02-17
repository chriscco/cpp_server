#include "../inc/event_loop.h"

EventLoop::EventLoop() : _epoll(nullptr), _quit(false) {
    _epoll = new Epoll();
}
EventLoop::~EventLoop() {
    delete _epoll;
}

void EventLoop::loop() {
    while (!_quit) {
        std::vector<Channel*> channels = _epoll->poll_events(-1);
        for (auto& ch : channels) {
            ch->handleEvent();
        }
    }
}

/**
 * @brief 在 EventLoop 中注册自己的channel
 * @param 当前需要加入epoll的channel
 */
void EventLoop::updateChannel(Channel* ch) {
    _epoll->updateChannel(ch);
}

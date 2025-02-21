#include "../inc/event_loop.h"

EventLoop::EventLoop() {
    _epoll = std::make_unique<Epoll>();
}
EventLoop::~EventLoop() {}

void EventLoop::loop() const {
    while (true) {
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
void EventLoop::updateChannel(Channel* ch) const {
    _epoll->updateChannel(ch);
}
void EventLoop::deleteChannel(Channel* ch) const {
    _epoll->deleteChannel(ch);
}

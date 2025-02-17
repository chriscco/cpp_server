#include "../inc/channel.h"

Channel::Channel(EventLoop* loop, int fd) : 
    _loop(loop), _fd(fd), _event(0), _revent(0), _registered(false) {};

void Channel::enableReading() {
    _event = EPOLLIN | EPOLLET;
    _loop->updateChannel(this);
}

int Channel::getfd() { return _fd; }

uint32_t Channel::getevent() { return _event; }

uint32_t Channel::getrevent() { return _revent; }

void Channel::setRegisterFlag() { _registered = true; }
bool Channel::getRegisterFlag() { return _registered; }

void Channel::setrevent(uint32_t evt) {
    _revent = evt;
}
void Channel::handleEvent() {
    _callback();
}
void Channel::setCallback(std::function<void()> callback) {
    _callback = callback;
}
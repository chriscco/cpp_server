#include "../inc/channel.h"

Channel::Channel(EventLoop* loop, int fd) : 
    _loop(loop), _fd(fd), _event(0), _revent(0), _registered(false) {};

Channel::~Channel() {
    if (_fd != -1) {
        close(_fd);
        _fd = 1;
    }
}

/**
 * @brief EPOLLPRI 代表接收紧急事件
 * EPOLL被默认设置为LT(水平触发)模式
 */
void Channel::enableReading() {
    _event |= EPOLLIN | EPOLLPRI;
    _loop->updateChannel(this);
}
void Channel::setET() {
    _event |= EPOLLET;
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
    _loop->addThread(_callback);
}
void Channel::setCallback(std::function<void()> callback) {
    _callback = callback;
}
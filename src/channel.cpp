#include "../inc/channel.h"

Channel::Channel(EventLoop* loop, int fd) : 
    _loop(loop), _fd(fd), _event(0), _ready(0),
     _registered(false) {};

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
    _loop->updateChannel(this);
}

int Channel::getfd() { return _fd; }

uint32_t Channel::getevent() { return _event; }

uint32_t Channel::getready() { return _ready; }

void Channel::setReady(uint32_t r) { _ready = r; } 
void Channel::setRegisterFlag(bool in) { _registered = in; }
bool Channel::getRegisterFlag() { return _registered; }

void Channel::handleEvent() {
    if (_ready & (EPOLLIN | EPOLLPRI)) {
        _readCallback();
    } 
    if (_ready & EPOLLOUT) {
        _writeCallback();
    }
}
void Channel::setReadCallback(std::function<void()> callback) {
    _readCallback = callback;
}
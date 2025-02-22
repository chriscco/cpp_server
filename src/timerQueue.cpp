#include "../inc/timerQueue.h"

TimerQueue::TimerQueue(EventLoop* loop) : _loop(loop) {
    createTimerfd();
    _channel = std::make_unique<Channel>(_loop, _timerfd);
    _channel->setReadCallback(std::bind(&TimerQueue::handleRead, this));
    _channel->enableReading();
}

void TimerQueue::createTimerfd() {
    _timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    errif(_timerfd == -1, "timerfd create error");
}

TimerQueue::~TimerQueue() {
    _loop->deleteChannel(_channel.get());
    close(_timerfd);
    for (const auto& entry : _timers) {
        delete entry.second;
    }
}

void TimerQueue::readTimerFd() {
    uint64_t read_byte;
    ssize_t readn = ::read(_timerfd, &read_byte, sizeof(read_byte));
    errif(readn != sizeof(read_byte), "TimerQueue::ReadTimerFd read error");
}

void TimerQueue::handleRead() {
    readTimerFd();
    _active_timers.clear();
    
    auto end = _timers.lower_bound(Entry(TimeStamp::Now(), reinterpret_cast<Timer *>(UINTPTR_MAX)));
    _active_timers.insert(_active_timers.end(), _timers.begin(), end);

    _timers.erase(_timers.begin(), end);
    for (const auto &entry : _active_timers)
    {
        entry.second->run();
    }
    resetTimers();
}
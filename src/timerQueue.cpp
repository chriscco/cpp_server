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
    for (const auto &entry : _active_timers) {
        entry.second->run();
    }
    resetTimers();
}


void TimerQueue::addTimer(TimeStamp timestamp, std::function<void()>&& cb, double interval){
    Timer* timer = new Timer(timestamp, std::move(cb), interval);

    if (insert(timer)) {
        resetTimerFd(timer);
    }
}

bool TimerQueue::insert(Timer * timer){
    bool reset_instantly = false;
    if(_timers.empty() || timer->expiration() < _timers.begin()->first){
        reset_instantly = true;
    }
    _timers.emplace(std::move(Entry(timer->expiration(), timer)));
    return reset_instantly;
}


void TimerQueue::resetTimers() {
    for (auto& entry: _active_timers) {
        if ((entry.second)->repeat()) {
            auto timer = entry.second;
            timer->restart(TimeStamp::Now());
            insert(timer);
        } else {
            delete entry.second;
        }
    } 

    if (!_timers.empty()) {
        resetTimerFd(_timers.begin()->second);
    }
}


void TimerQueue::resetTimerFd(Timer *timer){
    struct itimerspec new_;
    struct itimerspec old_;
    memset(&new_, '\0', sizeof(new_));
    memset(&old_, '\0', sizeof(old_));

    int64_t micro_seconds_dif = timer->expiration().microseconds() - TimeStamp::Now().microseconds();
    if (micro_seconds_dif < 100){
        micro_seconds_dif = 100;
    }

    new_.it_value.tv_sec = static_cast<time_t>(
        micro_seconds_dif / kMicrosecond2Second);
    new_.it_value.tv_nsec = static_cast<long>((
        micro_seconds_dif % kMicrosecond2Second) * 1000);
    int ret = ::timerfd_settime(_timerfd, 0, &new_, &old_);
    
    assert(ret != -1);
    (void)ret;
}
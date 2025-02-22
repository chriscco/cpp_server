#pragma once 
#include <memory>
#include <set>
#include <sys/timerfd.h>
#include "common.h"
#include "../inc/timer.h"
#include "../inc/channel.h"
#include "../inc/error_handler.h"

class TimerQueue {
    public:
    DISALLOW_COPY_MOVE(TimerQueue);
    TimerQueue(EventLoop*);
    ~TimerQueue();

    void createTimerfd(); // 创建timerfd

    void readTimerFd(); // 读取timerfd事件
    void handleRead(); // timerfd可读时，调用

    void resetTimerFd(Timer*); // 重新设置timerfd超时时间，关注新的定时任务
    void resetTimers(); // 将具有重复属性的定时器重新加入队列

    bool insert(Timer*); // 将定时任务加入队列
    void addTimer(TimeStamp, std::function<void()>&&, double); // 添加一个定时任务

private:
    typedef std::pair<TimeStamp, Timer*> Entry;

    EventLoop *_loop;
    int _timerfd;
    std::unique_ptr<Channel> _channel; 

    std::set<Entry> _timers; // 定时器集合
    std::vector<Entry> _active_timers; // 激活的定时器
};
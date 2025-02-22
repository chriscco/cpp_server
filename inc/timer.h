#pragma once
#include <functional>

#include "timeStamp.h"
#include "common.h"
class Timer {
public:
    DISALLOW_COPY_MOVE(Timer);
    Timer(TimeStamp, std::function<void()>&&, double);

    void restart(TimeStamp);

    void run() const;
    TimeStamp expiration() const;
    bool repeat() const;

private:
    TimeStamp _expiration; // 定时器的绝对时间
    std::function<void()> _callback; // 到达时间后进行回调
    double _interval; // 如果重复，则重复间隔
    bool _repeat;
};
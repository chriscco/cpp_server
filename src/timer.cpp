#include "../inc/timer.h"
#include "../inc/timeStamp.h"

Timer::Timer(TimeStamp timestamp, std::function<void()>&&cb, double interval = 0.0)
    : _expiration(timestamp),
      _callback(std::move(cb)),
      _interval(interval),
      _repeat(interval > 0.0){};
    
void Timer::restart(TimeStamp now){
    _expiration = TimeStamp::AddTime(now, _interval);
}


void Timer::run() const{
    _callback();
}

TimeStamp Timer::expiration() const { return _expiration; }

bool Timer::repeat() const { return _repeat; }
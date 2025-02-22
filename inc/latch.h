#pragma once
#include <mutex>
#include <thread>
#include <condition_variable>
#include "common.h"
class Latch {
private:
    int _count;
    std::mutex _mutex;
    std::condition_variable _condition_var;
public:
    DISALLOW_COPY_MOVE(Latch);
    explicit Latch(int count) : _count(count) {};
    void wait() {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_count > 0) {
            _condition_var.wait(lock);
        } 
    }

    void notify() {
        std::unique_lock<std::mutex> lock(_mutex);
        --_count;
        if (_count == 0) {
            _condition_var.notify_all();
        }
    }
};
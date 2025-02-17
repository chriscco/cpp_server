#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
private:
    std::vector<std::thread> _threads;
    std::queue<std::function<void()>> _tasks;
    std::mutex _mutex_lock;
    std::condition_variable _condition_var;
    bool _stop;
public:
    ThreadPool(int size = std::thread::hardware_concurrency());
    ~ThreadPool();
    void add(std::function<void()> task);
};
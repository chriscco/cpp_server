#include "../inc/threadpool.h"

ThreadPool::ThreadPool(int size) : _stop(false) {
    for (int i = 0; i < size; i++) {
        _threads.emplace_back(std::thread([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(_mutex_lock);
                    _condition_var.wait(lock, [this]() {
                        return _stop || !_tasks.empty();
                    });
                    if (_stop && _tasks.empty()) return; // 当前没有可执行的任务并且线程池已经被停止
                    task = _tasks.front();
                    _tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(_mutex_lock);
        _stop = true;
    }
    _condition_var.notify_all();
    // 阻塞主线程等待目标线程执行完毕
    for (auto& th : _threads) {
        if (th.joinable()) {
            th.join();
        }
    }
}
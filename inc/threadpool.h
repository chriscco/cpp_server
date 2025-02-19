#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <pthread.h>
#include <future> 

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

    template<class F, class... Args>
    auto add(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>;
};

template<class F, class... Args>
auto ThreadPool::add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(_mutex_lock);
        if (_stop) {
            throw std::runtime_error("enqueue on stopped threadpool");
        }
        // (*task)() 调用 packaged_task, 实际执行封装在其中的f(args...);
        _tasks.emplace([task]() { (*task)(); });
    }
    _condition_var.notify_one();
    return res; 
}
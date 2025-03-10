
#include "../inc/eventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop)
    : _mainReactor(loop),
      _thread_num(0),
      _next(0){};

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start(){
    for (int i = 0; i < _thread_num; ++i){
        std::unique_ptr<EventLoopThread> ptr = std::make_unique<EventLoopThread>(i % _NUMA_NODES_NUM);
        _threads.emplace_back(std::move(ptr));
        _loops.push_back(_threads.back()->startLoop());
    }
}

EventLoop *EventLoopThreadPool::nextloop(){
    EventLoop *ret = _mainReactor;
    if (!_loops.empty()){
        ret = _loops[_next++];
        if (_next == static_cast<int>(_loops.size())){
            _next = 0;
        }
    }
    return ret;
}

void EventLoopThreadPool::setThreadSize(int thread_nums){
    _thread_num = thread_nums;
}

void EventLoopThreadPool::bind_numa() {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    int cpu_count = numa_num
}
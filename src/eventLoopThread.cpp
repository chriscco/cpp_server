#include "../inc/eventLoopThread.h"

EventLoopThread::EventLoopThread(int numa_node) : _loop(nullptr), _numa_node(numa_node) {};

EventLoopThread::~EventLoopThread() {};


EventLoop *EventLoopThread::startLoop(){
    // 绑定当前线程的所执行的函数，并创建子线程
    // 在这个线程中创建EventLoop.
    _thread = std::thread(std::bind(&EventLoopThread::threadFunction, this));

    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_loop == NULL){
            _condition_var.wait(lock); // 当IO线程未创建LOOP时，阻塞
        }
        // 将IO线程创建的_loop赋给主线程。
        loop = _loop;
    
    }
    // 返回创建好的线程。
    return loop;
}

void EventLoopThread::threadFunction(){

    bind_numa();

    // 由IO线程创建EventLoop对象
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _loop = &loop; // 获取子线程的地址
        _condition_var.notify_one(); // _loop被创建成功，发送通知，唤醒主线程。
    }

    _loop->loop(); // 开始循环，直到析构
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _loop = nullptr;
    }
}

void EventLoopThread::bind_numa() {
    /* 位图结构, 用于标识一个线程可以在哪些 CPU 核心上运行 */
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    pthread_t native_handle = _thread.native_handle();

    int cpu_count = numa_num_configured_cpus();
    for (int i = 0; i < cpu_count; i++) {
        if (numa_node_of_cpu(i) == _numa_node) {
            CPU_SET(i, &cpuset);
        }
    }

    /* 线程将只会在这些 CPU 核心上运行 */
    errif(pthread_setaffinity_np(native_handle, sizeof(cpuset), &cpuset) < 0, 
            "error binding affinity");
}
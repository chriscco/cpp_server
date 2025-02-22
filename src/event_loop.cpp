#include "../inc/event_loop.h"

EventLoop::EventLoop() : _callingFunc(false) {
    _epoll = std::make_unique<Epoll>();
    _fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    _channel = std::make_unique<Channel>(this, _fd);
    
    _channel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    _channel->enableReading();
}
EventLoop::~EventLoop() {
    deleteChannel(_channel.get());
    ::close(_fd);
}

void EventLoop::loop() {
    _thread_id = CurrentThread::tid();
    while (true) {
        std::vector<Channel*> channels = _epoll->poll_events(-1);
        for (auto& ch : channels) {
            ch->handleEvent();
        }
        runList();
    }
}

bool EventLoop::ifInMainThread() {
    return _thread_id == CurrentThread::tid();
}

void EventLoop::queueFunction(std::function<void()> func) {
    {
        /** 加锁，保证线程同步 */ 
        std::unique_lock<std::mutex> lock(_mutex);
        _todoList.emplace_back(std::move(func));
    }

    if (!ifInMainThread() || _callingFunc) {
        uint64_t write_one_byte = 1;  
        ssize_t write_size = ::write(_fd, &write_one_byte, sizeof(write_one_byte));
        (void) write_size;
        assert(write_size == sizeof(write_one_byte));
    } 
}

void EventLoop::runList() {
    _callingFunc = true;

    std::vector<std::function<void()>> functors;
    {
        /** 加锁 保证线程同步 */ 
        std::unique_lock<std::mutex> lock(_mutex); 
        functors.swap(_todoList);
    }
    for(const auto& func: functors) {
        func();
    }

    _callingFunc = false;
}

void EventLoop::runFunction(std::function<void()> func) {
    if (ifInMainThread()) {
        func();
    } else {
        queueFunction(func);
    }
}
/**
 * @brief 在 EventLoop 中注册自己的channel
 * @param 当前需要加入epoll的channel
 */
void EventLoop::updateChannel(Channel* ch) const {
    _epoll->updateChannel(ch);
}
void EventLoop::deleteChannel(Channel* ch) const {
    _epoll->deleteChannel(ch);
}
void EventLoop::handleRead(){
    // 用于唤醒EventLoop
    uint64_t read_one_byte = 1;
    ssize_t read_size = ::read(_fd, &read_one_byte, sizeof(read_one_byte));
    (void) read_size;
    assert(read_size == sizeof(read_one_byte));
    return;
}
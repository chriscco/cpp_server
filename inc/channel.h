#pragma once 

#include <sys/epoll.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <functional>

#include "error_handler.h"
#include "epoll.h"
#include "event_loop.h"

class Epoll;
class EventLoop;
/**
 * @brief 每个Channel对象绑定一个epoll_fd, 并负责管理
 * @class Channel 
 */
class Channel {
private:
    /** 指向Epoll实例的指针, 表示当前Channel由哪一个epoll实例管理 */
    EventLoop* _loop;
    /** 当前Channel负责管理的fd */
    int _fd;
    /** 表示希望监听的事件, 如可读可写或错误 */
    uint32_t _event;
    /** 存储当前epoll_wait()返回的具体事件 */
    uint32_t _ready;
    /** 判断当前fd是否已经加入epoll红黑树 */
    bool _registered;
    std::function<void()> _readCallback;
    std::function<void()> _writeCallback;
public: 
    Channel(EventLoop*, int);
    ~Channel();

    void enableReading();
    void setET();
    void handleEvent();

    void setReady(uint32_t);
    void setRegisterFlag(bool in = true);
    bool getRegisterFlag();

    int getfd();
    uint32_t getevent();
    uint32_t getready();

    void setReadCallback(std::function<void()>);
};
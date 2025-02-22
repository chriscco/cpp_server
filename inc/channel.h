#pragma once 

#include <sys/epoll.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <functional>

#include "error_handler.h"
#include "epoll.h"
#include "event_loop.h"
#include "common.h"

class Epoll;
class EventLoop;
/**
 * @brief 每个Channel对象绑定一个epoll_fd, 并负责管理
 * @class Channel 
 */
class Channel {
private:
    DISALLOW_COPY_MOVE(Channel)
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
    /** 判断当前弱指针是否已经指向共享指针 */
    bool _tied;
    std::function<void()> _readCallback;
    std::function<void()> _writeCallback;
    std::weak_ptr<void> _tie;
public: 
    Channel(EventLoop*, int);
    ~Channel();

    void enableReading();
    void enableWriting();
    void enableET();
    void handleEvent();
    void handleEventWithGuard();

    void setReady(uint32_t);
    void setRegisterFlag(bool in = true);
    bool getRegisterFlag();

    void tieWeak(const std::shared_ptr<void>& ptr);

    int getfd() const ;
    uint32_t getevent() const ;
    uint32_t getready() const;

    void setWriteCallback(std::function<void()>&&);
    void setReadCallback(std::function<void()>&&);
};
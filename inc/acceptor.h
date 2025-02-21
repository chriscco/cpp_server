#pragma once 
#include <functional>
#include <cassert>
#include "event_loop.h"
#include "channel.h"
#include "common.h"

class EventLoop;
class Channel;
class Socket;
class InetAddr;
/**
 * @brief Acceptor类只负责监听和接收新连接
 * @class Acceptor
 */
class Acceptor {
private:
    EventLoop* _loop;
    std::unique_ptr<Channel> _channel;
    int _fd;
    std::function<void(int)> _newConnectionCallback;
public:
    DISALLOW_COPY_MOVE(Acceptor);
    Acceptor(EventLoop*, const char*, const int);
    ~Acceptor();
    void acceptConnection();
    void setNewConnectionCallback(std::function<void(int)> const&);
    void create();
    void bind(const char*, const int);
    void listen();
};
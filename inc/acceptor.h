#pragma once 
#include <functional>
#include "event_loop.h"
#include "channel.h"
#include "socket.h"
#include "inetaddr.h"

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
    Channel* _channel;
    Socket* _socket;
    InetAddr* _inetaddr;
public:
    Acceptor(EventLoop*);
    ~Acceptor();
    void acceptConnection();
    std::function<void(Socket*)> newConnectionCallback;
    void setNewConnectionCallback(std::function<void(Socket*)>);
};
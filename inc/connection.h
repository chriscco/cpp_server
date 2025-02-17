#pragma once
#include <functional>
#include "acceptor.h"

#define READ_SIZE 1024

class EventLoop;
class Socket;
class Channel;

class Connection {
private: 
    EventLoop* _loop;
    Channel* _channel;
    Socket* _socket;
    std::function<void(Socket*)> _deleteCallback;
public:
    Connection(EventLoop*, Socket*);
    ~Connection();

    void echo(int sockfd);
    void setDeleteCallback(std::function<void(Socket*)>);
};
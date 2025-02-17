#pragma once
#include <functional>
#include "acceptor.h"
#include "buffer.h"

#define READ_SIZE 1024

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
private: 
    EventLoop* _loop;
    Channel* _channel;
    Socket* _socket;
    Buffer* _readBuffer;
    std::string* _inBuffer;
    std::function<void(Socket*)> _deleteCallback;
public:
    Connection(EventLoop*, Socket*);
    ~Connection();

    void echo(int sockfd);
    void setDeleteCallback(std::function<void(Socket*)>);
};
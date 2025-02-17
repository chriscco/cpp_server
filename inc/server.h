#pragma once
#include <functional>

#include "../inc/socket.h"
#include "../inc/channel.h"
#include "../inc/event_loop.h"
#include "../inc/acceptor.h"
#define READ_SIZE 1024

class EventLoop;
class Socket;
class Channel;
class Acceptor;

class Server {
private:
    EventLoop* _loop;
    Acceptor* _acceptor;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket*);
};


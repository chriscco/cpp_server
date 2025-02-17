#pragma once
#include <functional>

#include "../inc/socket.h"
#include "../inc/channel.h"
#include "../inc/event_loop.h"
#define READ_SIZE 1024

class EventLoop;
class Socket;
class Channel;

class Server {
private:
    EventLoop* _loop;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket*);
};


#pragma once
#include <functional>
#include <map> 

#include "../inc/socket.h"
#include "../inc/channel.h"
#include "../inc/event_loop.h"
#include "../inc/acceptor.h"
#include "../inc/connection.h"
#define READ_SIZE 1024

class EventLoop;
class Socket;
class Channel;
class Acceptor;
class Connection;

class Server {
private:
    EventLoop* _loop;
    Acceptor* _acceptor;
    std::map<int, Connection*> _connections; 
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket*);
    void deleteConnection(Socket*);
};


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

/**
 * @class Server 
 * @brief 采用主从Reactor模式，让sub-reactor在线程中运行
 */
class Server {
private:
    EventLoop* _mainReactor; // 只负责接收新连接, 之后分发给sub-reactor
    Acceptor* _acceptor;
    std::map<int, Connection*> _connections; 
    std::vector<EventLoop*> _subReactor;
    ThreadPool* _pool;
    std::function<void(Connection*)> _onConnectionCallback;
public:
    Server(EventLoop*);
    ~Server();

    void newConnection(Socket*);
    void deleteConnection(Socket*);
    void onConnect(std::function<void(Connection*)>);
};


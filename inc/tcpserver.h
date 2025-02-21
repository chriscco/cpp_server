#pragma once
#include <functional>
#include <map> 

#include "../inc/channel.h"
#include "../inc/event_loop.h"
#include "../inc/acceptor.h"
#include "../inc/tcpconnection.h"
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
    int _nextConnId;
    std::unique_ptr<EventLoop> _mainReactor; // 只负责接收新连接, 之后分发给sub-reactor
    std::unique_ptr<Acceptor> _acceptor;
    std::unordered_map<int, Connection*> _connections; 
    std::vector<std::unique_ptr<EventLoop>> _subReactor;
    std::unique_ptr<ThreadPool> _pool;

    std::function<void(Connection*)> _onConnectionCallback;
    std::function<void(Connection*)> _onMessageCallback;
public:
    DISALLOW_COPY_MOVE(Server);
    Server(const char*, const int);
    ~Server();
    
    void start();
    void newConnection(int);
    void closeConnection(int);

    void setOnConnCallback(std::function<void(Connection*)>&&);
    void setOnMessageCallback(std::function<void(Connection*)>&&);
};


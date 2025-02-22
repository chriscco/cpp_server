#pragma once
#include <functional>
#include <map> 

#include "channel.h"
#include "event_loop.h"
#include "acceptor.h"
#include "tcpconnection.h"
#include "eventLoopThread.h"
#include "eventLoopThreadPool.h"

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
    EventLoop* _mainReactor; // 只负责接收新连接, 之后分发给sub-reactor
    std::unique_ptr<Acceptor> _acceptor;
    std::unordered_map<int, std::shared_ptr<Connection>> _connections; 
    std::unique_ptr<EventLoopThreadPool> _pool;

    std::function<void(const std::shared_ptr<Connection>&)> _onConnectionCallback;
    std::function<void(const std::shared_ptr<Connection>&)> _onMessageCallback;
public:
    DISALLOW_COPY_MOVE(Server);
    Server(EventLoop*, const char*, const int);
    ~Server();
    
    void start();
    inline void handleNewConnection(int);
    inline void handleCloseConnection(const std::shared_ptr<Connection>&);
    /** 进一步封装, 确保是由主线程的_mainReactor进行关闭 */
    inline void handleCloseConnectionGuard(const std::shared_ptr<Connection>&);

    void setOnConnCallback(std::function<void(const std::shared_ptr<Connection>&)>&&);
    void setOnMessageCallback(std::function<void(const std::shared_ptr<Connection>&)>&&);

    void setThreadSize(int);
};


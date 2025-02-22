#include "../inc/tcpserver.h"

/**
 * @brief 新建Server实例时创建一个Acceptor实例，将Server的newConnection函数绑定至
 * Acceptor实例的acceptConnection函数，当新的连接被触发时，Acceptor通过callback函数调用
 * newConnection，其中具体的逻辑和操作仍由Server类管理
 * @param eventLoop
 */
Server::Server(EventLoop* loop, const char* ip, const int port) : 
                    _mainReactor(loop), _nextConnId(1) {
    _mainReactor = std::make_unique<EventLoop>();
    _acceptor = std::make_unique<Acceptor>(_mainReactor.get(), ip, port);

    /** callback被调用时自动调用newConnection(), 并传入socketfd */
    std::function<void(int)> callback = std::bind(&Server::handleNewConnection, this, std::placeholders::_1);
    _acceptor->setNewConnectionCallback(callback);

    int size = static_cast<int>(std::thread::hardware_concurrency());
    _pool = std::make_unique<ThreadPool>(size);
    for (int i = 0; i < size; i++) {
        std::unique_ptr<EventLoop> subReactor = std::make_unique<EventLoop>();
        _subReactor.emplace_back(std::move(subReactor));
    }
}

Server::~Server() {};

void Server::start() {
    for (auto& sub : _subReactor) {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, sub.get());
        _pool->add(std::move(sub_loop));
    }
    _mainReactor->loop();
}

/**
 * @brief 使用hash算法实现随机调度，防止sub-reactor出现分配不均
 * @param socket socket
 */
void Server::handleNewConnection(int fd) {
    assert(fd != -1);
    uint64_t random = fd % _subReactor.size();

    std::shared_ptr<Connection> connection = std::make_shared<Connection>(_subReactor[random].get(), fd, _nextConnId);
    std::function<void(const std::shared_ptr<Connection>&)> callback = std::bind(&Server::handleCloseConnection, this, std::placeholders::_1);
    connection->setConnectionCallback(std::move(_onConnectionCallback));
    connection->setCloseCallback(std::move(callback));
    connection->setMessageCallback(std::move(_onMessageCallback));
    _connections[fd] = connection;
    
    _nextConnId++;
    if (_nextConnId >= 1000) { _nextConnId = 1; }

    connection->connectionEstablish();
}

void Server::handleCloseConnection(const std::shared_ptr<Connection>& conn) {
    _mainReactor->runFunction(std::bind(&Server::handleCloseConnectionGuard, this, conn));
}

void Server::handleCloseConnectionGuard(const std::shared_ptr<Connection>& conn) {
    auto it = _connections.find(conn->fd());
    assert(it != _connections.end());
    _connections.erase(_connections.find(conn->fd()));

    EventLoop *loop = conn->getLoop();
    loop->queueFunction(std::bind(&Connection::connectionDestruct, conn));
}

void Server::setOnConnCallback(std::function<void(const std::shared_ptr<Connection>&)>&& callback) {
    _onConnectionCallback = std::move(callback);
}
void Server::setOnMessageCallback(std::function<void(const std::shared_ptr<Connection>&)>&& callback) {
    _onMessageCallback = std::move(callback);
}
#include "../inc/tcpserver.h"

/**
 * @brief 新建Server实例时创建一个Acceptor实例，将Server的newConnection函数绑定至
 * Acceptor实例的acceptConnection函数，当新的连接被触发时，Acceptor通过callback函数调用
 * newConnection，其中具体的逻辑和操作仍由Server类管理
 * @param eventLoop
 */
Server::Server(const char* ip, const int port) : _nextConnId(1) {
    _mainReactor = std::make_unique<EventLoop>();
    _acceptor = std::make_unique<Acceptor>(_mainReactor.get(), ip, port);

    /** callback被调用时自动调用newConnection(), 并传入socketfd */
    std::function<void(int)> callback = std::bind(&Server::newConnection, this, std::placeholders::_1);
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
void Server::newConnection(int fd) {
    if (fd != -1) {
        int random = fd % _subReactor.size();
        Connection* connection = new Connection(_subReactor[random].get(), fd, _nextConnId);
        std::function<void(int)> callback = std::bind(&Server::closeConnection, this, std::placeholders::_1);
        connection->setCloseCallback(std::move(callback));
        connection->setConnectionCallback(std::move(_onConnectionCallback));
        _connections[fd] = connection;
    }
    _nextConnId++;
    if (_nextConnId >= 1000) { _nextConnId = 1; }
}

void Server::closeConnection(int fd) {
    if (fd != -1) {
        auto it = _connections.find(fd);
        if (it != _connections.end()) {
            Connection* conn = _connections[fd];
            _connections.erase(fd);
            close(fd);
            conn = nullptr;
        }
    }
}

void Server::setOnConnCallback(std::function<void(Connection*)>&& callback) {
    _onConnectionCallback = std::move(callback);
}
void Server::setOnMessageCallback(std::function<void(Connection*)>&& callback) {
    _onMessageCallback = std::move(callback);
}
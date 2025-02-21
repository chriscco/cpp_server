#include "../inc/server.h"

/**
 * @brief 新建Server实例时创建一个Acceptor实例，将Server的newConnection函数绑定至
 * Acceptor实例的acceptConnection函数，当新的连接被触发时，Acceptor通过callback函数调用
 * newConnection，其中具体的逻辑和操作仍由Server类管理
 * @param eventLoop
 */
Server::Server(EventLoop* eventLoop) : _mainReactor(eventLoop), _acceptor(nullptr) {
    _acceptor = new Acceptor(_mainReactor);

    /** callback被调用时自动调用newConnection(), 并传入socket */
    std::function<void(Socket*)> callback = std::bind(&Server::newConnection, this, std::placeholders::_1);
    _acceptor->setNewConnectionCallback(callback);

    int size = static_cast<int>(std::thread::hardware_concurrency());
    _pool = new ThreadPool(size);
    for (int i = 0; i < size; i++) {
        _subReactor.emplace_back(new EventLoop());
    }
    for (auto& sub : _subReactor) {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, sub);
        _pool->add(sub_loop);
    }
}

Server::~Server() {
    delete _acceptor;
    delete _pool;
}

/**
 * @brief 使用hash算法实现随机调度，防止sub-reactor出现分配不均
 * @param socket socket
 */
void Server::newConnection(Socket* socket) {
    if (socket->getfd() != -1) {
        int random = socket->getfd() % _subReactor.size();
        Connection* connection = new Connection(_subReactor[random], socket);
        std::function<void(Socket*)> callback = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        connection->setDeleteCallback(callback);
        connection->setConnectionCallback(_onConnectionCallback);
        _connections[socket->getfd()] = connection;
    }
}

void Server::deleteConnection(Socket* socket) {
    int sockfd = socket->getfd();
    if (sockfd != -1) {
        auto it = _connections.find(sockfd);
        if (it != _connections.end()) {
            Connection* conn = _connections[sockfd];
            _connections.erase(sockfd);
            close(sockfd);
            conn = nullptr;
        }
    }
}

void Server::onConnect(std::function<void(Connection*)> callback) {
    _onConnectionCallback = std::move(callback);
}
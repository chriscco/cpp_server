#include "../inc/server.h"

/**
 * @brief 新建Server实例时创建一个Acceptor实例，将Server的newConnection函数绑定至
 * Acceptor实例的acceptConnection函数，当新的连接被触发时，Acceptor通过callback函数调用
 * newConnection，其中具体的逻辑和操作仍由Server类管理
 * @param eventLoop
 */
Server::Server(EventLoop* eventLoop) : _loop(eventLoop), _acceptor(nullptr) {
    _acceptor = new Acceptor(_loop);

    /** callback被调用时自动调用newConnection(), 并传入socket */
    std::function<void(Socket*)> callback = std::bind(&Server::newConnection, this, std::placeholders::_1);
    _acceptor->setNewConnectionCallback(callback);
}

Server::~Server() {
    delete _acceptor;
}

void Server::newConnection(Socket* socket) {
    Connection* connection = new Connection(_loop, socket);
    std::function<void(int)> callback = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    connection->setDeleteCallback(callback);
    _connections[socket->getfd()] = connection;
}

void Server::deleteConnection(int sockfd) {
    if (sockfd != -1) {
        auto it = _connections.find(sockfd);
        if (it != _connections.end()) {
            Connection* conn = _connections[sockfd];
            _connections.erase(sockfd);
            close(sockfd);
            delete conn;
        }
    }
}
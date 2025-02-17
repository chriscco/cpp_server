#include "../inc/acceptor.h"

Acceptor::Acceptor(EventLoop* loop) : _loop(loop){
    _socket = new Socket();
    _inetaddr = new InetAddr("127.0.0.1", 8888);
    _socket->bind(_inetaddr);
    _socket->listen();
    _socket->setnonblocking();
    
    _channel = new Channel(_loop, _socket->getfd());

    std::function<void()> callback = std::bind(&Acceptor::acceptConnection, this);
    _channel->setCallback(callback);
    _channel->enableReading();
}

Acceptor::~Acceptor() {
    delete _socket;
    delete _inetaddr;
    delete _channel;
}

void Acceptor::acceptConnection() {
    newConnectionCallback(_socket);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> callback) {
    newConnectionCallback = callback;
}
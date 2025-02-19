#include "../inc/acceptor.h"

Acceptor::Acceptor(EventLoop* loop) : _loop(loop){
    _socket = new Socket();
    InetAddr* addr = new InetAddr("127.0.0.1", 8080);
    _socket->bind(addr);
    _socket->listen();
    // _socket->setnonblocking();
    
    _channel = new Channel(_loop, _socket->getfd());

    std::function<void()> callback = std::bind(&Acceptor::acceptConnection, this);
    _channel->setReadCallback(callback);
    _channel->enableReading();
    _channel->setUseThread(false);
    delete addr;
}

Acceptor::~Acceptor() {
    delete _socket;
    delete _channel;
}

void Acceptor::acceptConnection() {
    InetAddr* client_addr = new InetAddr();
    Socket* client_socket = new Socket(_socket->accept(client_addr));
    
    printf("new connection from fd: %d, addr: %s, port: %d\n", client_socket->getfd(), 
                inet_ntoa(client_addr->get_addr().sin_addr), ntohs(client_addr->get_addr().sin_port));
    client_socket->setnonblocking();
    newConnectionCallback(client_socket);
    delete client_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> callback) {
    newConnectionCallback = callback;
}
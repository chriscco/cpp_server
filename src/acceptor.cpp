#include "../inc/acceptor.h"

Acceptor::Acceptor(EventLoop* loop, const char* ip, const int port) : 
                                            _loop(loop), _fd(-1) {
    create();
    bind(ip, port);
    listen();
    _channel = std::make_unique<Channel>(_loop, _fd);
    std::function<void()> callback = std::bind(&Acceptor::acceptConnection, this);
    _channel->setReadCallback(callback);
    _channel->enableReading();
}

Acceptor::~Acceptor() {
    _loop->deleteChannel(_channel.get());
    ::close(_fd);
}

void Acceptor::create() {
    assert(_fd != -1);
    _fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    errif(_fd == -1, "socket creation error");
}
void Acceptor::bind(const char* ip, const int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    int ret = ::bind(_fd, (sockaddr*)&addr, sizeof(addr));
    errif(ret == -1, "bind error");
}
void Acceptor::listen() {
    assert(_fd != -1);
    errif(::listen(_fd, SOMAXCONN) == -1, "listen error");
}

void Acceptor::acceptConnection() {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    assert(_fd != -1);
    int client_fd = accept4(_fd, (sockaddr*)&client_addr, &addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    errif(client_fd == -1, "accept4 error");
    if (_newConnectionCallback) {
        _newConnectionCallback(client_fd);
    }
}

void Acceptor::setNewConnectionCallback(std::function<void(int)>const& callback) {
    _newConnectionCallback = std::move(callback);
}
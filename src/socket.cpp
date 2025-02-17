#include "../inc/socket.h"

Socket::Socket() : _fd(-1) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(_fd < 0, "socket error in Socket().");
}

Socket::Socket(int fd) : _fd(fd) {
    errif(fd < 0, "socket error in Socket(int).");
}

Socket::~Socket() {
    if (_fd != -1) {
        close(_fd);
        _fd = -1;
    }
}

void Socket::bind(InetAddr* addr) {
    int b = ::bind(_fd, (sockaddr*)&addr->get_addr(), addr->get_len());
    errif(b < 0, "bind error.");
}

int Socket::accept(InetAddr* addr) {
    int a = ::accept(_fd, (sockaddr*)&addr->get_addr(), &addr->get_len());
    errif(a < 0, "accept error.");
    return a;
}

void Socket::listen() {
    errif(::listen(_fd, SOMAXCONN) < 0, "listen error.");
}

void Socket::setnonblocking() {
    fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
}

int Socket::getfd() {
    return _fd;
}


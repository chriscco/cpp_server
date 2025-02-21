#include "../inc/socket.h"

Socket::Socket() : _fd(-1) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(_fd < 0, "socket error in Socket().");
}

Socket::Socket(int fd) : _fd(fd) {
    errif(_fd < 0, "socket error in Socket(int).");
}

Socket::~Socket() {
    if (_fd != -1) {
        close(_fd);
        _fd = -1;
    }
}

void Socket::bind(InetAddr* addr) {
    struct sockaddr_in address = addr->get_addr();
    socklen_t addr_len = addr->get_len();
    errif(::bind(_fd, (sockaddr*)&address, addr_len), "socket bind error.");
    addr->setInetAddr(address, addr_len);
}
    

int Socket::accept(InetAddr* addr) {
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    bzero(&address, addr_len);
    int client_sockfd = -1;
    if (fcntl(_fd, F_GETFL) & O_NONBLOCK) {
        while(true) {
            client_sockfd = ::accept(_fd, (sockaddr*)&address, &addr_len);
            if (client_sockfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                printf("waiting for connection\n");
                continue;
            } else if (client_sockfd == -1) {
                errif(true, "socket accept error in nonblock.");
            }
            else break;
        }
    } else {
        client_sockfd = ::accept(_fd, (sockaddr*)&address, &addr_len);
        errif(client_sockfd == -1, "socket accept error.");
    }
    addr->setInetAddr(address, addr_len);
    return client_sockfd;
}

void Socket::listen() {
    errif(::listen(_fd, SOMAXCONN) < 0, "listen error.");
}

void Socket::connect(InetAddr* addr) {
    struct sockaddr_in address = addr->get_addr();
    if (fcntl(_fd, F_GETFL) & EWOULDBLOCK) {
        while (true) {
            int ret = ::connect(_fd, (sockaddr*)&address, sizeof(address));
            if (ret == 0) break;
            else if (ret == -1 && (errno == EINPROGRESS)) {
                continue;
            } else if (ret == -1) {
                errif(true, "socket connect error in nonblock");
            }
        }
    } else { 
        int ret = ::connect(_fd, (sockaddr*)&address, sizeof(address));
        errif(ret == -1, "socket connect error.");
    }
}

void Socket::setnonblocking() {
    fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
}

int Socket::getfd() {
    return _fd;
}


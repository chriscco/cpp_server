#pragma once
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include "error_handler.hpp"

class InetAddr {
private: 
    struct sockaddr_in _addr;
    socklen_t _addr_len;
public:
    InetAddr();
    InetAddr(const char* ip, uint16_t port);
    ~InetAddr();

    struct sockaddr_in& get_addr();

    socklen_t& get_len();
};

struct sockaddr_in& InetAddr::get_addr() { return _addr; }

socklen_t& InetAddr::get_len() { return _addr_len; }

InetAddr::InetAddr() {
    bzero(&_addr, sizeof(_addr));
    _addr_len = sizeof(_addr);
}

InetAddr::InetAddr(const char* ip, uint16_t port) {
    bzero(&_addr, _addr_len);
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(ip);
    _addr.sin_port = htons(port);
    _addr_len = sizeof(_addr);
}
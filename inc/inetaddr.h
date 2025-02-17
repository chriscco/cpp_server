#pragma once
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include "error_handler.h"

class InetAddr {
private: 
    struct sockaddr_in _addr;
    socklen_t _addr_len;
public:
    InetAddr();
    InetAddr(const char* ip, uint16_t port);
    ~InetAddr();

    struct sockaddr_in get_addr();
    socklen_t get_len();

    void setInetAddr(sockaddr_in addr, socklen_t addr_len);
};
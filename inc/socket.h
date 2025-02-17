#pragma once
#include "inetaddr.h"
#include <unistd.h>

class Socket {
private:
    int _fd;
public:
    Socket();
    Socket(int);
    ~Socket();

    void bind(InetAddr*);
    int accept(InetAddr*);

    void listen();
    void setnonblocking() ;

    int getfd();
};


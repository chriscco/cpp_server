#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "inc/epoll.h"
#include "inc/error_handler.h"
#include "inc/inetaddr.h"
#include "inc/tcpserver.h"

#define MAX_EVENT 1024

/** 
 * 当前代码使用了ET边缘触发
 */
int main() {
    Server *server = new Server("127.0.0.1", 8080);

    server->setOnMessageCallback([](Connection *conn) {
        std::cout << "Message from client " << conn->id() 
            << " is " << conn->getReadBuffer()->c_str() << std::endl;
        conn->send(conn->getReadBuffer()->c_str()); 
    });

    server->start();

    delete server;
    return 0;
}

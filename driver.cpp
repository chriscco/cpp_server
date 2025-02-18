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
#include "inc/socket.h"
#include "inc/server.h"

#define MAX_EVENT 1024

/** 
 * 当前代码使用了ET边缘触发
 */
int main() {
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);
    loop->loop();
    delete server;
    delete loop;
    return 0;
}

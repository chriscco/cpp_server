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

#define MAX_EVENT 1024


void handleReadEvent(int sockfd) {
    char buf[1024];
    while (true) {
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("message from client: %s with sockfd: %d\n", buf, sockfd);
            write(sockfd, buf, sizeof(buf)); // 将数据写回客户端
        } else if(read_bytes == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading\n");
            continue;
        }
        // 非阻塞io，以下判断是否完成读取, EAGAIN, EWOULDBLOCK表示没有数据可读 
        else if (read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            printf("finished reading: errno: %s\n", errno);
            break;
        } else if (read_bytes == 0) {
            printf("fd: %d has disconnected\n", sockfd);
            close(sockfd);
        }
    }
}

/** 
 * 当前代码使用了ET边缘触发
 */
int main() {
    Socket* socket = new Socket();
    InetAddr* server_addr = new InetAddr("127.0.0.1", 8888);
    socket->bind(server_addr);
    socket->listen();
    
    Epoll* epoll = new Epoll();
    socket->setnonblocking();
    epoll->add_fd(socket->getfd(), EPOLLIN | EPOLLET);

    Channel* channel = new Channel(epoll, socket->getfd());
    channel->enableReading();

    while (true) {
        // timeout:: -1: 阻塞模式. 0: 非阻塞模式. 返回nfds个触发的事件
        std::vector<Channel*> events_queue = epoll->poll_events();
        int nfds = events_queue.size();

        for (int i = 0; i < nfds; ++i) {
            int chfd = events_queue[i]->getfd();
            if (chfd == socket->getfd()) { //发生的事件来自服务器的socket fd, 表示有新的连接
                InetAddr* client_addr = new InetAddr();
                Socket* client_socket = new Socket(socket->accept(client_addr));
                
                client_socket->setnonblocking();
                epoll->add_fd(client_socket->getfd(), EPOLLIN | EPOLLET);

                Channel* client_channel = new Channel(epoll, client_socket->getfd());
                client_channel->enableReading();
            } else if (chfd & EPOLLIN) { //读取当前fd发生的事件类型并使用&判断是否包含EPOLLIN
                handleReadEvent(chfd);
            }
        }
    }
    delete server_addr;
    delete socket;
    return 0;
}

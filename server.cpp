#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "inc/error_handler.hpp"

#define MAX_EVENT 1024

void setNonBlocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}


/** 
 * 当前代码使用了ET边缘触发
 */
int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd < 0, "socket creation error.");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0, 
        "binding error.");
    errif(listen(sockfd, SOMAXCONN) < 0, "listen error.");

    int epoll_fd = epoll_create1(0);
    errif(epoll_fd < 0, "epoll fd creation error");

    /*
    event[]: 用于epoll_wait()接收事件数组
    evt: 用于epoll_ctl()注册/修改/删除单个file descriptor
    */
    struct epoll_event event[MAX_EVENT], evt;
    bzero(&event, sizeof(event));
    bzero(&evt, sizeof(evt));

    evt.data.fd = sockfd;
    evt.events = EPOLLIN | EPOLLET;
    setNonBlocking(sockfd);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &evt);

    while (true) {
        // -1: 阻塞模式. 0: 非阻塞模式. 返回nfds个触发的事件
        int nfds = epoll_wait(epoll_fd, event, MAX_EVENT, -1);
        errif(nfds < 0, "epoll_wait() error.");

        for (int i = 0; i < nfds; ++i) {
            if (event[i].data.fd == sockfd) { //发生的事件来自服务器的socket fd, 表示有新的连接
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                bzero(&client_addr, client_addr_len);
                int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
                errif(client_sockfd < 0, "client accept error.");
                evt.data.fd = client_sockfd;
                evt.events = EPOLLIN | EPOLLET;
                
                setNonBlocking(client_sockfd);
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &evt);
            } else if (event[i].events & EPOLLIN) { //读取当前fd发生的事件类型并使用&判断是否包含EPOLLIN
                char buf[1024];
                ssize_t read_bytes = read(event[i].data.fd, buf, sizeof(buf));
                if (read_bytes > 0) {
                    printf("message from client: %s with sockfd: %d\n", buf, event[i].data.fd);
                    write(event[i].data.fd, buf, sizeof(buf)); // 将数据写回客户端
                } else if(read_bytes == -1 && errno == EINTR){  //客户端正常中断、继续读取
                    printf("continue reading\n");
                    continue;
                }
                // 非阻塞io，以下判断是否完成读取 
                else if (read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                    printf("finished reading: errno: %s\n", errno);
                    break;
                } else if (read_bytes == 0) {
                    printf("fd: %d has disconnected\n", event[i].data.fd);
                    close(event[i].data.fd);
                }
            }        
        }
    }
    close(sockfd);
}
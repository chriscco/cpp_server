#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "inc/error_handler.hpp"

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

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, client_addr_len);

    int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
    errif(client_sockfd < 0, "socket accept error.");

    printf("New connection from client IP: %s, PORT: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (true) {
        char buf[1024];
        bzero(&buf, sizeof(buf));

        ssize_t read_bytes = read(client_sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("message from client: %s with sockfd: %d\n", buf, sizeof(buf));
            write(client_sockfd, buf, sizeof(buf)); // 将数据写回客户端
        } else if (read_bytes == 0) {
            printf("client disconnected");
            close(client_sockfd);
            break;
        } else if (read_bytes < 0) {
            close(client_sockfd);
            errif(true, "client read error.");
        }
    }
    close(sockfd);
}
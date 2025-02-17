#include "../inc/server.h"

Server::Server(EventLoop* eventLoop) : _loop(eventLoop) {
    Socket* socket = new Socket();
    InetAddr* server_addr = new InetAddr("127.0.0.1", 8888);
    socket->bind(server_addr);
    socket->listen();
    socket->setnonblocking();

    Channel* channel = new Channel(_loop, socket->getfd());
    /** callback被调用时自动调用newConnection(), 并传入socket */
    std::function<void()> callback = std::bind(&Server::newConnection, this, socket);
    channel->setCallback(callback);
    channel->enableReading();
}

void Server::handleReadEvent(int sockfd) {
    char buf[READ_SIZE];
    while (true) {
        bzero(&buf, sizeof(buf));
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

void Server::newConnection(Socket* socket) {
    InetAddr* client_addr = new InetAddr(); // 会导致内存泄漏
    Socket* client_socket = new Socket(socket->accept(client_addr));
    
    client_socket->setnonblocking();

    Channel* client_channel = new Channel(_loop, client_socket->getfd());
    std::function<void()> callback = std::bind(&Server::handleReadEvent, this, client_socket->getfd());

    client_channel->setCallback(callback);
    client_channel->enableReading();
}
#include "../inc/connection.h"

Connection::Connection(EventLoop* loop, Socket* socket) : _loop(loop), _socket(socket) {
    _channel = new Channel(_loop, _socket->getfd());
    std::function<void()> callback = std::bind(&Connection::echo, this, _socket->getfd());
    _channel->setCallback(callback);
    _channel->enableReading();
}

Connection::~Connection() {
    delete _socket;
    delete _channel;
}

void Connection::echo(int sockfd) {
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
            // close(sockfd);
            _deleteCallback(_socket);
            break;
        }
    }
}

void Connection::setDeleteCallback(std::function<void(Socket*)> callback) {
    _deleteCallback = callback;
}
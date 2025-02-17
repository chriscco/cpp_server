#include "../inc/connection.h"

Connection::Connection(EventLoop* loop, Socket* socket) : _loop(loop), _socket(socket), 
                                        _channel(nullptr), _inBuffer(new std::string()) {
    _channel = new Channel(_loop, _socket->getfd());
    std::function<void()> callback = std::bind(&Connection::echo, this, _socket->getfd());
    _channel->setCallback(callback);
    _channel->enableReading();
    _readBuffer = new Buffer();
}

Connection::~Connection() {
    delete _socket;
    delete _channel;
}

void Connection::echo(int sockfd) {
    char buf[1024];     //这个buf大小无所谓
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            _readBuffer->append(buf, bytes_read);
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once\n");
            printf("message from client fd %d: %s\n", sockfd, _readBuffer->c_str());
            errif(write(sockfd, _readBuffer->c_str(), _readBuffer->size()) == -1, "socket write error");
            _readBuffer->clear();
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            _deleteCallback(_socket);
            break;
        }
    }
}

void Connection::setDeleteCallback(std::function<void(Socket*)> callback) {
    _deleteCallback = callback;
}
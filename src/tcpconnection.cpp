#include "../inc/connection.h"

Connection::Connection(EventLoop* loop, Socket* socket) : _loop(loop), _socket(socket) {
    if (_loop != nullptr) {
        _channel = new Channel(_loop, _socket->getfd());
        _channel->setET();
        _channel->enableReading();
    }
    _readBuffer = new Buffer();
    _writeBuffer = new Buffer();
    _state = State::CONNECTED;
}

Connection::~Connection() {
    if (_loop != nullptr) {
        delete _channel;
    }
    delete _socket;
    delete _readBuffer;
    delete _writeBuffer;
}

void Connection::read() {
    assert(_state == State::CONNECTED);
    _readBuffer->clear();
    readNonBlocking();
}

void Connection::write() {
    assert(_state == State::CONNECTED);
    writeNonBlocking();
    _writeBuffer->clear();
}

void Connection::closeConnection() { _deleteCallback(_socket); }

void Connection::readNonBlocking() {
    int sockfd = _socket->getfd();
    char buf[1024];  // 这个buf大小无所谓
    while (true) {   // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
      memset(buf, 0, sizeof(buf));
      ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
      if (bytes_read > 0) {
        _readBuffer->append(buf, bytes_read);
      } else if (bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
        printf("continue reading\n");
        continue;
      } else if (bytes_read == -1 &&
                 ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
        break;
      } else if (bytes_read == 0) {  // EOF，客户端断开连接
        printf("read EOF, client fd %d disconnected\n", sockfd);
        _state = State::CLOSED;
        break;
      } else {
        printf("Other error on client fd %d\n", sockfd);
        _state = State::CLOSED;
        break;
      }
    }
}

void Connection::writeNonBlocking() {
    int sockfd = _socket->getfd();
    char buf[_writeBuffer->size()];
    memcpy(buf, _writeBuffer->c_str(), _writeBuffer->size());
    int data_size = _writeBuffer->size();
    int data_left = data_size;
    while (data_left > 0) {
      ssize_t bytes_write = ::write(sockfd, buf + data_size - data_left, data_left);
      if (bytes_write == -1 && errno == EINTR) {
        printf("continue writing\n");
        continue;
      }
      if (bytes_write == -1 && errno == EAGAIN) {
        break;
      }
      if (bytes_write == -1) {
        printf("Other error on client fd %d\n", sockfd);
        _state = State::CLOSED;
        break;
      }
      data_left -= bytes_write;
    }
}

Socket* Connection::getSocket() { return _socket; }
void Connection::setWriteBuffer(const char* str) { _writeBuffer->setBuffer(str); }

std::string Connection::getState() { return std::to_string(_state); };
Buffer* Connection::getReadBuffer() { return _readBuffer; };
Buffer* Connection::getWriteBuffer() { return _writeBuffer; };
const char* Connection::readBuffer() { return _readBuffer->c_str(); };
const char* Connection::writeBuffer() { return _writeBuffer->c_str(); };

void Connection::setDeleteCallback(std::function<void(Socket*)> const &callback) {
    _deleteCallback = callback;
}

void Connection::setConnectionCallback(std::function<void(Connection*)> const &callback) {
    _onConnectionCallback = callback;
    _channel->setReadCallback([this]() { _onConnectionCallback(this); });
}
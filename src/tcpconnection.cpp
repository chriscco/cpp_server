#include "../inc/tcpconnection.h"

Connection::Connection(EventLoop* loop, int connfd, int connid) : _loop(loop), 
                        _connfd(connfd), _connid(connfd) {
    if (_loop != nullptr) {
        _channel = std::make_unique<Channel>(_loop, _connfd);
        _channel->enableET();
        _channel->enableReading();
    }
    _readBuffer = std::make_unique<Buffer>();
    _writeBuffer = std::make_unique<Buffer>();
    _state = State::CONNECTED;
}

Connection::~Connection() {
    ::close(_connfd);
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

void Connection::send(std::string& msg) {
    setWriteBuffer(msg.c_str());
    write();
}
void Connection::send(const char* msg) {
    setWriteBuffer(msg);
    write();
}

void Connection::closeConnection() {
    if (_state != State::DISCONNECTED) {
        _state = State::DISCONNECTED;
        if (_onCloseCallback) {
          _onCloseCallback(_connfd);
        }
    }
}

void Connection::readNonBlocking() {
    char buf[1024];  // 这个buf大小无所谓
    while (true) {   // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
      memset(buf, 0, sizeof(buf));
      ssize_t bytes_read = ::read(_connfd, buf, sizeof(buf));
      if (bytes_read > 0) {
        _readBuffer->append(buf, bytes_read);
      } else if (bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
        printf("continue reading\n");
        continue;
      } else if (bytes_read == -1 &&
                 ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
        break;
      } else if (bytes_read == 0) {  // EOF，客户端断开连接
        printf("read EOF, client fd %d disconnected\n", _connfd);
        closeConnection();
        break;
      } else {
        printf("Other error on client fd %d\n", _connfd);
        closeConnection();
        break;
      }
    }
}

void Connection::writeNonBlocking() {
    char buf[_writeBuffer->size()];
    memcpy(buf, _writeBuffer->c_str(), _writeBuffer->size());
    int data_size = _writeBuffer->size();
    int data_left = data_size;
    while (data_left > 0) {
      ssize_t bytes_write = ::write(_connfd, buf + data_size - data_left, data_left);
      if (bytes_write == -1 && errno == EINTR) {
        printf("continue writing\n");
        continue;
      }
      if (bytes_write == -1 && errno == EAGAIN) {
        break;
      }
      if (bytes_write == -1) {
        printf("Other error on client fd %d\n", _connfd);
        closeConnection();
        break;
      }
      data_left -= bytes_write;
    }
}

void Connection::setWriteBuffer(const char* str) { _writeBuffer->setBuffer(str); }

std::string Connection::getState() { return std::to_string(_state); };
Buffer* Connection::getReadBuffer() { return _readBuffer.get(); };
Buffer* Connection::getWriteBuffer() { return _writeBuffer.get(); };

int Connection::fd() const { return _connfd; }
int Connection::id() const { return _connid; }

void Connection::setCloseCallback(std::function<void(int)>&& callback) {
    _onCloseCallback = std::move(callback);
}

void Connection::setConnectionCallback(std::function<void(Connection*)>&& callback) {
    _onConnectionCallback = callback;
    _channel->setReadCallback([this]() { _onConnectionCallback(this); });
}
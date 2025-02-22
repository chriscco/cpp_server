#pragma once
#include <functional>
#include <cassert>
#include "acceptor.h"
#include "buffer.h"
#include "common.h"

#define READ_SIZE 1024

class EventLoop;
class Socket;
class Channel;
class Buffer;

/**
 * @brief std::enable_shared_from_this 允许在Connection类中创建自身的shared_ptr
 */
class Connection : std::enable_shared_from_this<Connection> {
private: 
    enum State {
        INVALID = 1,
        CONNECTED,
        DISCONNECTED,
    };
    EventLoop* _loop;
    int _connfd;
    int _connid;
    std::unique_ptr<Channel> _channel;
    std::unique_ptr<Buffer> _readBuffer;
    std::unique_ptr<Buffer> _writeBuffer;
    /** 参数类型采用 const std::shared_ptr<Connection> & 防止导致引用计数器增加 */
    std::function<void(const std::shared_ptr<Connection> &)> _onCloseCallback;
    std::function<void(const std::shared_ptr<Connection> &)> _onConnectionCallback;
    std::function<void(const std::shared_ptr<Connection> &)> _onMessageCallback;
    State _state{State::INVALID};

    void writeNonBlocking();
    void readNonBlocking();
public:
    DISALLOW_COPY_MOVE(Connection);
    Connection(EventLoop*, int, int);
    ~Connection();

    void connectionEstablish();
    void connectionDestruct();

    void setWriteBuffer(const char* str);
    /** 建立连接时调用 */
    void setConnectionCallback(std::function<void(const std::shared_ptr<Connection>&)>&&);
    /** 关闭连接时调用 */
    void setCloseCallback(std::function<void(const std::shared_ptr<Connection>&)>&&);
    /** 接收到信息时调用 */
    void setMessageCallback(std::function<void(const std::shared_ptr<Connection>&)>&&);

    void handleConnection();
    void handleClose();

    std::string getState();
    Buffer* getReadBuffer();
    Buffer* getWriteBuffer();
    EventLoop* getLoop();

    void read();
    void write();
    void send(std::string&);
    void send(const char*, int);
    void send(const char*);
    int fd() const;
    int id() const;
};
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

class Connection {
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
    std::function<void(int)> _onCloseCallback;
    std::function<void(Connection*)> _onConnectionCallback;
    State _state{State::INVALID};

    void writeNonBlocking();
    void readNonBlocking();
public:
    DISALLOW_COPY_MOVE(Connection);
    Connection(EventLoop*, int, int);
    ~Connection();

    void setWriteBuffer(const char* str);
    void setCloseCallback(std::function<void(int)>&& callback);
    void setConnectionCallback(std::function<void(Connection*)>&& callback);
    void closeConnection();

    std::string getState();
    Buffer* getReadBuffer();
    Buffer* getWriteBuffer();

    void read();
    void write();
    void send(std::string&);
    void send(const char*, int);
    void send(const char*);
    int fd() const;
    int id() const;
};
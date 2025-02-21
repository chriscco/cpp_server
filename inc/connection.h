#pragma once
#include <functional>
#include <cassert>
#include "acceptor.h"
#include "buffer.h"

#define READ_SIZE 1024

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
private: 
    enum State {
        INVALID = 1,
        HAND_SHAKING,
        CONNECTED,
        FAILED, 
        CLOSED,
    };
    EventLoop* _loop;
    Channel* _channel{nullptr};
    Socket* _socket;
    Buffer* _readBuffer{nullptr};
    Buffer* _writeBuffer{nullptr};
    std::function<void(Socket*)> _deleteCallback;
    std::function<void(Connection*)> _onConnectionCallback;
    State _state{State::INVALID};

    void writeNonBlocking();
    void readNonBlocking();
public:
    Connection(EventLoop*, Socket*);
    ~Connection();

    void setWriteBuffer(const char* str);
    void setDeleteCallback(std::function<void(Socket*)> const& callback);
    void setConnectionCallback(std::function<void(Connection*)> const& callback);
    void closeConnection();

    Socket* getSocket();
    std::string getState();
    Buffer* getReadBuffer();
    Buffer* getWriteBuffer();
    const char* readBuffer();
    const char* writeBuffer();

    void read();
    void write();
};
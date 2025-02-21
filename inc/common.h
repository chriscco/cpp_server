class Connection;
class Buffer;
class Server;
class Socket;
class InetAddr;
class EventLoop;
class Epoll;
class Acceptor;
class Channel;
class ThreadPool;

#define DISALLOW_COPY_CONSTRUCT(cname) \
    cname(const cname&) = delete; \
    cname& operator=(const cname&) = delete;
#define DISALLOW_MOVE_CONSTRUCT(cname) \
    cname(const cname&&) = delete; \
    cname& operator=(const cname&&) = delete;
#define DISALLOW_COPY_MOVE(cname) \ 
    DISALLOW_COPY_CONSTRUCT(cname) \ 
    DISALLOW_MOVE_CONSTRUCT(cname)
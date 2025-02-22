#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "inc/epoll.h"
#include "inc/error_handler.h"
#include "inc/inetaddr.h"
#include "inc/tcpserver.h"
#include "inc/currentThread.h"

#define MAX_EVENT 1024

class EchoServer{
    public:
        EchoServer(EventLoop *loop, const char *ip, const int port);
        ~EchoServer();

        void start();
        void onConnection(const std::shared_ptr<Connection> & conn);
        void onMessage(const std::shared_ptr<Connection> & conn);

        void setThreadSize(int thread_nums);

    private:
        Server _server;
};

EchoServer::EchoServer(EventLoop *loop, const char *ip, const int port) :  _server(loop, ip, port){
    _server.setOnConnCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    _server.setOnMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1));
};
EchoServer::~EchoServer(){};

void EchoServer::start(){
    _server.start();
}

void EchoServer::onConnection(const std::shared_ptr<Connection> & conn){
    // 获取接收连接的Ip地址和port端口
    int clnt_fd = conn->fd();
    struct sockaddr_in peeraddr;
    socklen_t peer_addrlength = sizeof(peeraddr);
    getpeername(clnt_fd, (struct sockaddr *)&peeraddr, &peer_addrlength);

    std::cout << CurrentThread::tid()
              << " EchoServer::OnNewConnection : new connection "
              << "[fd#" << clnt_fd << "]"
              << " from " << inet_ntoa(peeraddr.sin_addr) << ":" << ntohs(peeraddr.sin_port)
              << std::endl;
};

void EchoServer::onMessage(const std::shared_ptr<Connection> & conn){
    // std::cout << CurrentThread::tid() << " EchoServer::onMessage" << std::endl;
    if (conn->getState() == "CONNECTED")
    {
        std::cout << CurrentThread::tid() << "Message from clent " << conn->getReadBuffer()->c_str() << std::endl;
        conn->send(conn->getReadBuffer()->c_str());
        conn->handleClose();
    }
}

void EchoServer::setThreadSize(int thread_nums) { _server.setThreadSize(thread_nums); }

int main(int argc, char *argv[]){
    int port;
    if (argc <= 1)
    {
        port = 8080;
    }else if (argc == 2){
        port = atoi(argv[1]);
    }else{
        printf("error");
        exit(0);
    }
    int size = std::thread::hardware_concurrency();
    EventLoop *loop = new EventLoop();
    EchoServer *server = new EchoServer(loop, "127.0.0.1", port);
    server->setThreadSize(size);
    server->start();

    // delete loop;
    // delete server;
    return 0;
}
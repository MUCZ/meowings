#ifndef CHATSERVER
#define CHATSERVER

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/ThreadLocalSingleton.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <set>
#include <stdio.h>
#include <unistd.h>
#include "chatservice.hh"

using namespace muduo;
using namespace muduo::net;
using namespace std;
using namespace std::placeholders;

class ChatServer{
    public:
        ChatServer(EventLoop * loop, 
                   const InetAddress& listenAddr)
                : server_(loop,listenAddr,"MeowingServer")
                {
                    server_.setConnectionCallback(
                        bind(&ChatServer::onConnection, this, _1));
                    server_.setMessageCallback(
                        bind(&ChatServer::onMessage, this, _1,_2,_3));
                }
        

        void setThreadNum(int numThreads){ server_.setThreadNum(numThreads);}

        void start()
        {
            server_.start();
        }

    private:
        void onConnection(const TcpConnectionPtr& conn);
        void onMessage(const TcpConnectionPtr &conn, Buffer* buffer,Timestamp time);

        TcpServer server_;
};

#endif /* CHATSERVER */

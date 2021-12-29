#ifndef CHATCLIENT
#define CHATCLIENT
#include <mutex>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <chrono>
#include <exception>
#include <map>
#include <vector>
#include <string>
#include <optional>

#include <muduo/base/Logging.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Condition.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/TcpClient.h>
#include "json.hpp"

#include "public.hh"
#include "friendmodel.hh"
#include "offlinemessagemodel.hh"
#include "user.hh"
#include "groupmodel.hh"
#include "group.hh"
#include "log_redirected.hh"


using namespace muduo;
using namespace muduo::net;
using namespace std;
using namespace std::placeholders;
using json = nlohmann::json; 

class ChatClient {
public:
    ChatClient(EventLoop* loop, const InetAddress& serverAddr,bool tcpNoDelay = true)
    : client_(loop,serverAddr,"ChatClient"),loop_(loop),tcpNoDelay_(tcpNoDelay),mutexForCondition_(),ackCondition_(mutexForCondition_)
    {
        client_.setConnectionCallback(bind(&ChatClient::onConnection,this,_1));
        client_.setMessageCallback(bind(&ChatClient::onMessage,this,_1,_2,_3));
        client_.enableRetry();
    }

    void connect() { client_.connect(); }
    void disconnect() { client_.disconnect(); }
    
    void waitForConnectionEstablished(){ 
        if(!connection_ || !connection_->connected()){ // wait for connection established
            cout << "connecting to server... "<<endl;
            connectionCountDown_.wait();
        }
        assert(connection_ && connection_->connected());
    }

    void assertNotInLoopThread(){ assert(!loop_->isInLoopThread()); }
    void assertOnline() { assert(user_.has_value() && user_->getId());}

    int reg(string name, string pwd) ;
    
    bool login(int userid,string pwd);

    void logout();

    void addFriend(int friendid);

    int createGroup(string& name,string& desc);

    void joinGroup(int groupid);

    void oneChat(int friendid,string& msg);

    void groupChat(int groupid, string& msg);

    void update();

    void showFriends() const ;
    void showGroups() const;
    void showCurrentUserInfo() const;

private:
    void onConnection(const TcpConnectionPtr & conn);

    void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time);

    TcpClient client_;
    EventLoop* loop_;

    TcpConnectionPtr connection_ ;  
    mutex connMutex_;

    const bool tcpNoDelay_;

    CountDownLatch connectionCountDown_{1}; // connection_established

    MutexLock mutexForCondition_;
    Condition ackCondition_;
    int ackWaited_{};
    json ack_;      

    optional<User> user_;
    vector<User> friendList_;
    vector<Group> groupList_;
};



#endif /* CHATCLIENT */

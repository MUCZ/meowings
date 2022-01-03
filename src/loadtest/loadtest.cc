#include <mutex>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <exception>
#include <map>
#include <vector>
#include <string>
#include <optional>

#include <muduo/base/Logging.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/Atomic.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Condition.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoopThreadPool.h>
#include "json.hpp"

#include "public.hh"
#include "friendmodel.hh"
#include "offlinemessagemodel.hh"
#include "user.hh"
#include "groupmodel.hh"
#include "group.hh"
#include "log_redirected.hh"
#include "dummyUI.hh"


using namespace muduo;
using namespace muduo::net;
using namespace std;
using namespace std::placeholders;
using json = nlohmann::json; 


int g_connections = 0;
int sent = 0;
AtomicInt32 g_aliveConnections;
AtomicInt32 g_messagesReceived;
Timestamp g_startTime; 
bool g_startTimeSet{false};
std::vector<Timestamp> g_receiveTime;
EventLoop* g_loop;
std::function<void()> g_statistic;
class Loadtest_ChatClient : noncopyable{
    public:
        Loadtest_ChatClient(EventLoop* loop,InetAddress serverAddr)
        :   connection_(),
            client_(loop,serverAddr,"loadtest_chat"),
            loop_(loop)
        {
            client_.setConnectionCallback(bind(&Loadtest_ChatClient::onConnection,this,_1));
            client_.setMessageCallback(bind(&Loadtest_ChatClient::onMessage,this,_1,_2,_3));
        }
        void connect() { client_.connect();}
        void disconnect() { client_.disconnect(); }
        Timestamp receiveTime() const {return receiveTime_;}

    private:
        void onConnection(const TcpConnectionPtr& conn){
            // cout<< conn->localAddress().toHostPort() << "-> " << conn->peerAddress().toHostPort() <<" is "<<(conn->connected()? "UP":"DOWN")<<endl;

            if(conn->connected()){
                connection_ = conn;
                connection_->setTcpNoDelay(true);
                if(g_aliveConnections.incrementAndGet() == g_connections){
                    cout<<"all connected! "<<endl;
                    // loop_->runAfter(3.0,bind(&Loadtest_ChatClient::send,this));
                }
                loop_->runAfter(5.0,bind(&Loadtest_ChatClient::send,this,conn));
            } else {
                connection_.reset();
            }
        }

        void onMessage(const TcpConnectionPtr &conn, Buffer * buffer, Timestamp time){
            string res = buffer->retrieveAllAsString();
            json response = json::parse(res);
            assert(response["msgid"]==HEART_BEAT_ACK);
           
            receiveTime_ = loop_->pollReturnTime();
            int received = g_messagesReceived.incrementAndGet();
            if(received == g_connections){
                Timestamp endTime = Timestamp::now();
                cout<<"all recieved "<<g_connections<<" in "<<timeDifference(endTime,g_startTime)<<endl;
                g_loop->queueInLoop(g_statistic);
            }
        }

        void send(TcpConnectionPtr& conn){
            if(!g_startTimeSet){
                g_startTime = Timestamp::now();
                g_startTimeSet = true;
            }
            json msg;
            msg["msgid"] = HEART_BEAT; 
            msg["time"] = g_startTime.toString();
            sent++;
            conn->send(msg.dump());
            cout<<"sent :"<< msg.dump()<<endl;

        }

        TcpConnectionPtr connection_;
        TcpClient client_;
        EventLoop* loop_;
        Timestamp receiveTime_;
};

void statistic(const std::vector<std::unique_ptr<Loadtest_ChatClient>>& clients){
    cout<<"Statistic "<<clients.size()<<endl;
    std::vector<double> seconds(clients.size());
    for(size_t i = 0; i<clients.size() ;++i){
        seconds[i] = timeDifference(clients[i]->receiveTime(),g_startTime);
    }
    std::sort(seconds.begin(), seconds.end());
    for (size_t i = 0; i < clients.size(); i += std::max(static_cast<size_t>(1), clients.size()/20)) {
        printf("%6zd%% %.6f\n", i*100/clients.size(), seconds[i]);
    }
    if (clients.size() >= 100){
        printf("%6d%% %.6f\n", 99, seconds[clients.size() - clients.size()/100]);
    }
    printf("%6d%% %.6f\n", 100, seconds.back());
    exit(1);
}

int main(int argc, char* argv[]){
    cout<<"pid = "<<getpid()<<endl;
    if(argc > 2){
        uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
        InetAddress serverAddr("127.0.0.1",port);
        g_connections = atoi(argv[2]);
        int threads = 0;
        if(argc >3 ){
            threads = atoi(argv[3]);
        }

        EventLoop loop;
        g_loop = &loop;
        EventLoopThreadPool loopPool(&loop,"chat-loadtest");
        loopPool.setThreadNum(threads);
        loopPool.start();

        g_receiveTime.reserve(g_connections);
        std::vector<std::unique_ptr<Loadtest_ChatClient>> clients(g_connections);
        g_statistic = std::bind(statistic,std::ref(clients));
        for(int i = 0; i<g_connections; ++i){
            clients[i]=(make_unique<Loadtest_ChatClient>(loopPool.getNextLoop(),serverAddr));
            clients[i]->connect();
            usleep(200);
        }
        loop.loop();
    } else {
        printf("Usage: %s <port> <connections> [threads] \n",argv[0]);
    }
}

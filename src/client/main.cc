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
#include "dummyUI.hh"


using namespace muduo;
using namespace muduo::net;
using namespace std;
using namespace std::placeholders;
using json = nlohmann::json; 



ChatClient* g_client{};

int main(int argc,char * argv[]){
    if(argc < 2){
        cout << "Usage : "<< argv[0] << " <port> "<<endl;
        exit(0);
    }
    set_log_output(argc,argv);
    LOG_INFO << "pid = " << getpid();

    EventLoopThread loopThread;
    // uint16_t port{};
    uint16_t port = atoi(argv[1]);
    InetAddress serverAddr("127.0.0.1",port); 
    ChatClient client(loopThread.startLoop(), serverAddr);
    g_client = &client;
    client.connect();
    cout << "Meowings is a simple Chat application." << endl ;
    while(1){
        mainMenu();
    }
}


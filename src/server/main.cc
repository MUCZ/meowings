#include "chatserver.hh"
#include <iostream>
using namespace std; 

int main(){ 
    EventLoop loop;
    InetAddress addr("127.0.0.1",6000);
    ChatServer server(&loop,addr);

    server.setThreadNum(4);
    server.start();

    loop.loop();
    return 1;
}
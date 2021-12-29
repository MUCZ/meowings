#include "chatserver.hh"
#include <iostream>
using namespace std; 

int main(int argc,char* argv[]){ 
    if(argc < 2){
        cout << "Usage : "<< argv[0] << " <port> "<<endl;
        exit(0);
    }
    EventLoop loop;
    uint16_t port = atoi(argv[1]);
    InetAddress addr("127.0.0.1",port);
    ChatServer server(&loop,addr);

    server.setThreadNum(4);
    server.start();

    loop.loop();
    return 1;
}
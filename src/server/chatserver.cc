#include "chatserver.hh"


void ChatServer::onMessage(const TcpConnectionPtr &conn, 
                            Buffer *buffer, 
                            Timestamp time){
    string buf = buffer->retrieveAllAsString(); // FIXME : 分包支持

    json js = json::parse(buf);

    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());

    msgHandler(conn,js,time);

}

void ChatServer::onConnection(const TcpConnectionPtr &conn){
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

    if (conn->connected()){
        // new connection
    } else {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}
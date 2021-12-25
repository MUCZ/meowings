#include "chatservice.hh"


// ctor
ChatService::ChatService(){ 
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg,this,_1,_2,_3)});
}

ChatService* ChatService::instance(){ 
    static ChatService service;
    return &service;
}


MsgHandler ChatService::getHandler(int msgid){
    auto it = _msgHandlerMap.find(msgid);
    if(it ==_msgHandlerMap.end()){
        // return a default error handler
        return [=](const TcpConnectionPtr, json, Timestamp){
            LOG_ERROR << "Invalid msgid: " << msgid << " Can't find handler "; 
        };
    } else {
      return _msgHandlerMap[msgid];
    }
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time){
    LOG_INFO << "logging...";
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time){
    // LOG_INFO << "registering...";
    string name = js["name"];
    string pwd= js["password"];
    User user(name,pwd);

    bool ret = _userModel.insert(user);
    if(ret){ // sucess
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["id"] = user.getId();
        response["errno"] = 0;
        response["errmsg"] = "";
        conn->send(response.dump());
    } else {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["id"] = user.getId();
        response["errno"] = 1;
        response["errmsg"] = "registration fails";
        conn->send(response.dump());
    }
}



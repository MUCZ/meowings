#include "chatservice.hh"
#include <vector>


// ctor
ChatService::ChatService(){ 
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg,this,_1,_2,_3)});
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHandlerMap.insert({LOGOUT_MSG, std::bind(&ChatService::logout,this,_1,_2,_3)});
    
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend,this,_1,_2,_3)});
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup,this,_1,_2,_3)});
    _msgHandlerMap.insert({JOIN_GROUP_MSG, std::bind(&ChatService::joinGroup,this,_1,_2,_3)});

    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat,this,_1,_2,_3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat,this,_1,_2,_3)});

}

ChatService* ChatService::instance(){ 
    static ChatService service;
    return &service;
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

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time){
    LOG_INFO << "logining...";
    int id = js["id"].get<int>();
    string pwd = js["password"];
    User user = _userModel.query(id);
    if(user.getId() == id && user.getPwd() == pwd){
        if(user.getState() == online){
            // re-login, user will be refused
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["erron"] = 2;
            response["errmsg"] = "this account is already online.";
            conn->send(response.dump());
        } else {
            // login sucess 

            // record connection
            {
                lock_guard<mutex> lock(_connMutex);                
                _userConnMap.insert({id,conn});
            }

            user.setState(online);
            _userModel.updateState(user);

            // login ack
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // check offline messages 
            vector<string> offline_msgs = _offlineMsgModel.retrieve(id);
            if(!offline_msgs.empty()){
                response["offlineMsg"] = offline_msgs;
                _offlineMsgModel.remove(id);

            }

            // return friends lists 
            vector<User> userVec = _friendModel.query(id);
            if(!userVec.empty()){
                vector<string> friends;
                for(const auto& f : userVec){
                    json people_js;
                    people_js["id"] = f.getId();
                    people_js["name"] = f.getName();
                    people_js["state"] = f.getState();
                    friends.emplace_back(people_js.dump());
                }
                response["friends"] = friends;
            }

            // return groups lists
            vector<Group> groupuserVec = _groupeModel.queryGroups(id);
            if(!groupuserVec.empty()){
                vector<string> groups;
                for(auto& g : groupuserVec){
                    json group_js;
                    group_js["id"] = g.getId();
                    group_js["groupename"] = g.getName();
                    group_js["groupdesc"] = g.getDesc();
                    vector<string> users_in_group;
                    for(auto &user : g.getUsers()){
                        json people_js;
                        people_js["id"] =user.getId();
                        people_js["name"] =user.getName();
                        people_js["state"] =user.getState();
                        people_js["role"] =user.getRole();
                        users_in_group.emplace_back(people_js.dump());
                    }
                    group_js["users"] = users_in_group;
                    groups.emplace_back(group_js.dump());
                }
            }

            conn->send(response.dump());
        }
    } else {
        // user don't exist or password wrong!
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["id"] = user.getId();
        response["errno"] = 1;
        response["errmsg"] = "id or password invalid!";
        conn->send(response.dump());
    }
}

void ChatService::logout(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if(it != _userConnMap.end()){
            _userConnMap.erase(it);
        }
    }
    
    User mock_user("","",offline,userid);
    _userModel.updateState(mock_user);
}


void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    int friendid = js["id"].get<int>();

    _friendModel.insert(userid,friendid);
}

void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    Group group(-1,name,desc);

    if(_groupeModel.createGroup(group)){
        _groupeModel.joinGroup(userid,group.getId(),creator);
    } //! todo : add reply
}

void ChatService::joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupeModel.joinGroup(userid,groupid,normies);
}


void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int toid = js["toid"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if(it != _userConnMap.end()){ // online && same server
            it->second->send(js.dump());
            return;
        }
    }

    User peer = _userModel.query(toid);
    if(peer.getState() == online){ // online && different server
        //  redis 
    } else { // offline 
        _offlineMsgModel.insert(toid,js.dump());
    }

}

void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();

    vector<int> useridVec = _groupeModel.queryGroupUsers(userid,groupid);

    lock_guard<mutex> lock(_connMutex);
    for(auto id : useridVec){
        auto it = _userConnMap.find(id);
        if(it != _userConnMap.end()){ // online && same server
            it->second->send(js.dump());
        } else {  
            User peer = _userModel.query(id);
            if(peer.getState() == online){ // online && different server
                //  redis 
            } else { // offline 
                _offlineMsgModel.insert(id,js.dump());
            }
        }
    }
}


void ChatService::clientCloseException(const TcpConnectionPtr& conn){
    //! todo : 改成conn的context，避免查找
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for(auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it){
            if( it -> second == conn){
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    if(user.getId()!=-1){
        user.setState(offline);
        _userModel.updateState(user);
    }
}

void ChatService::reset(){
    _userModel.resetState();
}

MsgHandler ChatService::getHandler(int msgid){
    auto it = _msgHandlerMap.find(msgid);
    if(it ==_msgHandlerMap.end()){
        // Handler not found, return a default one
        return [=](const TcpConnectionPtr, json, Timestamp){
            LOG_ERROR << "Invalid msgid: " << msgid << " Can't find handler "; 
        };
    } else {
      return _msgHandlerMap[msgid];
    }
}

#include "chatclient.hh"

string getCurrentTime()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}

int ChatClient::reg(string name, string pwd){
    // todo : check exisentance in server side
    // check thread & connection
    assertNotInLoopThread();
    waitForConnectionEstablished();

    // send reg msg
    json reg_js;
    reg_js["msgid"] = REG_MSG;
    reg_js["name"] = name;
    reg_js["password"] = pwd;
    connection_->send(reg_js.dump()); 
    cout << "reg msg sent, waiting for ack...."<<endl;

    //  wait for ack to know the id
    ackWaited_ = REG_MSG_ACK;
    ackCondition_.wait();
    if(ack_["msgid"].get<int>() == REG_MSG_ACK && ack_["errno"].get<int>() ==0){
        int id = ack_["id"].get<int>();
        return id;
    } else {
        LOG_ERROR << "reg fails:\n send: "<<reg_js.dump()<< "\n receive: " << ack_.dump() ;
        cout << "reg fails : "<< ack_["errmsg"].get<string>() <<endl;
        return 0;
    }

}

bool ChatClient::login(int userid, string pwd){  
    assertNotInLoopThread();
    waitForConnectionEstablished();

    json login_js; 
    login_js["msgid"] = LOGIN_MSG;
    login_js["id"] = userid;
    login_js["password"] = pwd;
    connection_->send(login_js.dump()); // ! todo 考虑加入长度
    // wait for login_ack

    ackWaited_ = LOGIN_MSG_ACK;
    ackCondition_.wait();
    assert(ack_.contains("msgid") && ack_.contains("errno"));
    if(ack_["msgid"].get<int>() == LOGIN_MSG_ACK && ack_["errno"].get<int>() == 0){
        assert(ack_["id"].get<int>()==userid);
        User user;
        user.setId(userid);
        user.setName(ack_["name"]);
        user.setPwd(pwd);
        user_ = user;
        cout << "login sucesses" <<endl;
        cout << "======================="<<endl;  
        // store friends 
        if(ack_.contains("friends")){
            vector<string> vec = ack_["friends"];
            for(const auto& str : vec){
                json js = json::parse(str);
                User f;
                f.setId(js["id"].get<int>());
                f.setName(js["name"]);
                f.setState(js["state"]);

                friendList_.emplace_back(f);
            }
        }

        // store groups
        if(ack_.contains("groups")){
            vector<string> vec = ack_["groups"];
            for(const auto& str : vec){
                json g_js = json::parse(str);
                Group g;
                g.setId(g_js["id"].get<int>());
                g.setName(g_js["groupname"]);
                g.setDesc(g_js["groupdesc"]);
                vector<string> vec_users = g_js["users"];
                for(const auto& str_user : vec_users){
                    json u_js = json:: parse(str_user);
                    GroupUser user;
                    user.setId(u_js["id"].get<int>());
                    user.setName(u_js["name"]);
                    user.setState(u_js["state"]);
                    user.setRole(u_js["role"]);
                    g.getUsers().emplace_back(user);
                }
                groupList_.emplace_back(g);
            }
        }
        showFriends();
        showGroups();

        // show offline msgs
        if(ack_.contains("offlineMsg")){
            // todo code might be reuse
            vector<string> vec = ack_["offlineMsg"];
            cout << "----OFFLINE-MSGS-------"<<endl;  
            for(const auto &str : vec){
                json js= json::parse(str);
                assert(js.contains("msg"));
                int msgtype = js["msgid"].get<int>();
                if(msgtype == ONE_CHAT_MSG){
                    cout << js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
                            << " said: " << js["msg"].get<string>() << endl;
                } else if (msgtype == GROUP_CHAT_MSG){
                    cout <<"group["<<js["groupid"] <<"]"<<js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
                            << " said: " << js["msg"].get<string>() << endl;
                } else {
                    cout <<"unsupported message : "<< js.dump() <<endl;
                }
            }
            cout << "-----------------------"<<endl;  
        }


        return true;
    } else {
        LOG_ERROR << "login fails:\n send:"<< login_js.dump()<<"\n receive: "<<ack_.dump();
        cout<< "login fails : "<< ack_["errmsg"]<<endl;
        return false;
    }
};

void ChatClient::logout(){ 
    assertNotInLoopThread();
    if(user_.has_value()){
        if(connection_ && connection_->connected()){
            json logout_js;
            logout_js["msgid"] = LOGOUT_MSG;
            logout_js["id"] = user_->getId();
            connection_->send(logout_js.dump());
            //! todo wait for ack ?
        }
        user_.reset(); 
    }
}

void ChatClient::addFriend(int friendid){ 
    assertNotInLoopThread();
    waitForConnectionEstablished();
    assertOnline();
    if(friendid == user_->getId()){
        cout <<friendid << " is yourself"<<endl;
        return;
    }
    for(const auto& f : friendList_){
        if(friendid == f.getId()){
            cout << friendid <<" is already your friends"<<endl;
            return;
        }
    }
    json add_f_js;
    add_f_js["msgid"] = ADD_FRIEND_MSG;
    add_f_js["id"] = user_->getId();
    add_f_js["friendid"] = friendid;
    connection_->send(add_f_js.dump());
    cout << "send request, waiting ..." << endl;

    // wait for ack
    ackWaited_ = ADD_FRIEND_MSG_ACK;
    ackCondition_.wait();
    if(ack_["msgid"].get<int>() == ADD_FRIEND_MSG_ACK && ack_["errno"].get<int>() ==0){
        assert(ack_["id"].get<int>() == user_->getId());
        assert(ack_["friendid"].get<int>() == friendid);
        cout << "ok" <<endl;
    } else {
        LOG_ERROR << "addFriends fails:\n send: "<<add_f_js.dump()<< "\n receive: " << ack_.dump() ;
        cout << "add friends fails : "<< ack_["errmsg"].get<string>() <<endl;
    }
}

int ChatClient::createGroup(string& name,string& desc){
    assertNotInLoopThread();
    waitForConnectionEstablished();
    assertOnline();

    json creat_group_js; 
    creat_group_js["msgid"] = CREATE_GROUP_MSG;
    creat_group_js["id"] = user_->getId();
    creat_group_js["groupname"] = name;
    creat_group_js["groupdesc"] = desc;
    connection_->send(creat_group_js.dump()); 
    
    // wait for ack
    ackWaited_ = CREATE_GROUP_MSG_ACK;
    ackCondition_.wait();
    if(ack_["msgid"].get<int>() == CREATE_GROUP_MSG_ACK && ack_["errno"].get<int>() ==0){
        assert(ack_["groupname"].get<string>() == name);
        assert(ack_["userid"].get<int>() == user_->getId());
        cout <<"group created, id : "<<ack_["groupid"].get<int>() << endl;
        return ack_["groupid"].get<int>();
    } else {
        LOG_ERROR << "createGroup fails:\n send: "<<creat_group_js.dump()<< "\n receive: " << ack_.dump() ;
        cout << "create group fails : "<< ack_["errmsg"].get<string>() <<endl;
        return -1;
    }
}

void ChatClient::joinGroup(int groupid){
    assertOnline();
    for(const auto& g: groupList_){
        if(groupid==g.getId()){
            cout <<"you are already in group" << groupid <<endl;
            return;
        }
    }
    json join_group_js;
    join_group_js["msgid"] = JOIN_GROUP_MSG;
    join_group_js["id"] = user_->getId();
    join_group_js["groupid"] = groupid;
    connection_->send(join_group_js.dump());
    
    ackWaited_ = JOIN_GROUP_MSG_ACK;
    ackCondition_.wait();
    if(ack_["msgid"].get<int>() == JOIN_GROUP_MSG_ACK && ack_["errno"].get<int>() ==0){
        assert(ack_["groupid"].get<int>() == groupid);
        assert(ack_["userid"].get<int>() == user_->getId());
        cout <<"group joined, id : "<<ack_["groupid"].get<int>()<<endl;
    } else {
        LOG_ERROR << "joinGroup fails:\n send: "<<join_group_js.dump()<< "\n receive: " << ack_.dump() ;
        cout << "joinGroup  fails : "<< ack_["errmsg"].get<string>() <<endl;
    }
}

void ChatClient::oneChat(int friendid,string& msg){ 
    assertNotInLoopThread();
    waitForConnectionEstablished();
    assertOnline();

    json chat_msg;
    chat_msg["msgid"] = ONE_CHAT_MSG;
    chat_msg["id"] = user_->getId();
    chat_msg["name"] = user_->getName();
    chat_msg["toid"] = friendid;
    chat_msg["msg"] = msg;
    chat_msg["time"] = getCurrentTime();
    connection_->send(chat_msg.dump());
    // todo for server : check friend exist
}


void ChatClient::groupChat(int groupid,string& msg){
    json chat_msg;
    chat_msg["msgid"] = GROUP_CHAT_MSG;
    chat_msg["id"] = user_->getId();
    chat_msg["name"] = user_->getName();
    chat_msg["groupid"] = groupid;
    chat_msg["msg"] = msg;
    chat_msg["time"] = getCurrentTime();
    connection_->send(chat_msg.dump());
    // todo for server: check user in group     
}

void ChatClient::update(){
    assertOnline();

    json update_js;
    update_js["msgid"] = RETRIEVAL_MSG;
    update_js["id"] = user_->getId();
    update_js["password"] = user_->getPwd();
    // todo : add verification
    connection_->send(update_js.dump());
    cout << "send update request, waiting ..." << endl;

    ackWaited_ = RETRIEVAL_MSG_ACK;
    ackCondition_.wait();
    assert(!ack_.empty() && ack_.contains("msgid"));
    if(ack_["msgid"].get<int>() == RETRIEVAL_MSG_ACK && ack_["errno"].get<int>() ==0){
        assert(ack_["id"].get<int>() == user_->getId());
        assert(ack_["name"].get<string>() == user_->getName());

        friendList_.clear();
        groupList_.clear();

        // store friends 
        if(ack_.contains("friends")){
            vector<string> vec = ack_["friends"];
            for(const auto& str : vec){
                json js = json::parse(str);
                User f;
                f.setId(js["id"].get<int>());
                f.setName(js["name"]);
                f.setState(js["state"]);

                friendList_.emplace_back(f);
            }
        }

        // store groups
        if(ack_.contains("groups")){
            vector<string> vec = ack_["groups"];
            for(const auto& str : vec){
                json g_js = json::parse(str);
                Group g;
                g.setId(g_js["id"].get<int>());
                g.setName(g_js["groupname"]);
                g.setDesc(g_js["groupdesc"]);
                vector<string> vec_users = g_js["users"];
                for(const auto& str_user : vec_users){
                    json u_js = json:: parse(str_user);
                    GroupUser user;
                    user.setId(u_js["id"].get<int>());
                    user.setName(u_js["name"]);
                    user.setState(u_js["state"]);
                    user.setRole(u_js["role"]);
                    g.getUsers().emplace_back(user);
                }
                groupList_.emplace_back(g);
            }
        }
        
        cout << "retrieval sucess ! "<<endl;
    } else {
        cout << "retrieval fails ! "<<endl;
    }
}

void ChatClient::showFriends() const {
    cout << "--------User info------"<<endl;  
    showCurrentUserInfo();
    cout << "--------Friends--------"<<endl;  
    cout << "  NAME  |  ID  | STATE"<<endl;  
    int count = 0;
    for(const auto& f : friendList_){
        cout << count++<<" "<< f.getName()<<" " << f.getId()<<" " << f.getState()<<endl;
    }
    cout << "-----------------------"<<endl;  
}

void ChatClient::showGroups() const{ 
    cout << "--------Groups---------"<<endl;  
    // cout << "   GROUPNAME  |  GROUPID  | DESCRIPTION"<<endl;  
    int count = 0;
    for(const auto& g : groupList_){
        cout << "["<<count++<<"]"<<" ID: "<<g.getId()<<" NAME: " <<g.getName();
        cout << "  \"" <<g.getDesc() <<"\" "<<endl;
        for(const auto& u : g.getUsers()){
            cout <<" "<< u.getName()<<" " << u.getId()<<" " << u.getState()<<endl;
        }
    }
    cout << "-----------------------"<<endl;  
}
void ChatClient::showCurrentUserInfo() const{ 
    if(user_.has_value()){
        cout<<"  Name    :"<<user_->getName()<<endl;
        cout<<"  Id      :"<<user_->getId()<<endl;
        // cout<<"  State        :"<<user_->getState()<<endl;
    } else {
        cout<<"login first " <<endl;
    }
}
void ChatClient::onConnection(const TcpConnectionPtr & conn){
    assert(loop_->isInLoopThread());
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

    lock_guard<mutex> lock(connMutex_);
    if (conn ->connected()){
        connection_ = conn;
        conn->setTcpNoDelay(tcpNoDelay_);
        connectionCountDown_.countDown(); 
    } else {
        connection_.reset();
        cout << "server closes the connection" << endl;
        abort();
    }
}

void ChatClient::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time){
    // FIXME : 解决分包问题
    string buf = buffer->retrieveAllAsString();
    json js;
    try{ 
        js = json::parse(buf);
    } catch (exception e){
        LOG_ERROR << "parse error, message received: "<< buf ;
        cerr << "parse error, message received: " <<buf<<endl;
        return;
    }
    
    if(js.contains("msgid") && js["msgid"].get<int>() == ackWaited_){
    // ack of a service : addfriend / joingroup / reg / logout
        ackCondition_.notify();
        ack_ = js;
        ackWaited_ = 0;
        return;
    } else {
    // a chat from a friend or a group
        assert(js.contains("msg"));
        int msgtype = js["msgid"].get<int>();
        cout << endl;
        if(msgtype == ONE_CHAT_MSG){
            cout << js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
                    << " said: " << js["msg"].get<string>() << endl;
        } else if (msgtype == GROUP_CHAT_MSG){
            cout <<"group["<<js["groupid"] <<"]"<<js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
                    << " said: " << js["msg"].get<string>() << endl;
        } else {
            cout <<"unsupported message : "<< js.dump() <<endl;
        }
        cout << " -> :" ;
        cout.flush();
    }
}

#ifndef CHATSERVICE
#define CHATSERVICE

#include <muduo/net/TcpConnection.h>
#include <muduo/base/Logging.h>
#include <unordered_map>
#include <functional>
#include <memory>
#include <mutex>

#include "json.hpp"

#include "public.hh"
#include "usermodel.hh"
#include "groupmodel.hh"
#include "offlinemessagemodel.hh"
#include "friendmodel.hh"

using namespace muduo; 
using namespace muduo::net; 
using namespace std;

using json = nlohmann::json;

// message handler type
using MsgHandler = std::function<void(const TcpConnectionPtr , json &js, Timestamp )>;

// ! Decoupling services and networks
// ! Singleton
class ChatService{
    public:
        static ChatService* instance();


        void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

        void login(const TcpConnectionPtr &conn, json &js, Timestamp time);

        void logout(const TcpConnectionPtr &conn, json &js, Timestamp time);

        void retrieval(const TcpConnectionPtr &conn, json &js, Timestamp time);

        void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

        void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

        void joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);


        void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
        
        void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);


        void clientCloseException(const TcpConnectionPtr &conn);

        // reset Users state to offline
        void reset();

        MsgHandler getHandler(int msgid);

    private:
        ChatService();
        unordered_map<int,MsgHandler> _msgHandlerMap; // const after init

        unordered_map<int,TcpConnectionPtr> _userConnMap; // ! shared by all threads
        mutex _connMutex;

        UserModel _userModel;
        OfflineMsgModel _offlineMsgModel;
        FriendModel _friendModel;
        GroupModel _groupModel;
};

#endif /* CHATSERVICE */

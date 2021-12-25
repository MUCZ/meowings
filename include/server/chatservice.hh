#ifndef CHATSERVICE
#define CHATSERVICE

#include <muduo/net/TcpConnection.h>
#include <muduo/base/Logging.h>
#include <unordered_map>
#include <functional>
#include <memory>

#include "json.hpp"

#include "public.hh"
#include "usermodel.hh"

using namespace muduo; 
using namespace muduo::net; 
using namespace std;

using json = nlohmann::json;

// 消息对应的回调
using MsgHandler = std::function<void(const TcpConnectionPtr , json &js, Timestamp )>;

// 聊天服务器业务类
// 解耦业务和网络
// 单例
class ChatService{
    public:
        // 获取单例
        static ChatService* instance();

        void login(const TcpConnectionPtr &conn, json &js, Timestamp time);

        void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

        MsgHandler getHandler(int msgid);
    private:
        ChatService();
        unordered_map<int,MsgHandler> _msgHandlerMap;
        UserModel _userModel;
};

#endif /* CHATSERVICE */

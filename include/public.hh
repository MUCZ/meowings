#ifndef PUBLIC
#define PUBLIC
#include <string>

// client & server message typecode protocal
enum EnMsgType{
    LOGIN_MSG = 1, // 1
    LOGIN_MSG_ACK, 
    REG_MSG ,
    REG_MSG_ACK ,
    LOGOUT_MSG , // 5 
    ADD_FRIEND_MSG,
    CREATE_GROUP_MSG,
    JOIN_GROUP_MSG,     
    ONE_CHAT_MSG,
    GROUP_CHAT_MSG // 10
};


// db definition
const std::string online = "online";
const std::string offline = "offline";

const std::string creator = "creator";
const std::string normies = "normal";

#endif /* PUBLIC */

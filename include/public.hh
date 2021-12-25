#ifndef PUBLIC
#define PUBLIC
#include <string>
// server 和 client 公用

enum EnMsgType{
    LOGIN_MSG = 1,
    LOGIN_MSG_ACK,
    REG_MSG ,
    REG_MSG_ACK
};

const std::string online = "online";
const std::string offline = "offline";

#endif /* PUBLIC */

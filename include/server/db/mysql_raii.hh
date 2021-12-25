#ifndef MYSQL_RAII
#define MYSQL_RAII
#include <string>
#include <muduo/base/Logging.h>
#include <mysql/mysql.h>

const std::string server = "127.0.0.1";
const std::string user = "root";
const std::string password = "mcz999416";
const std::string dbname = "chat";
const int MYSQL_LOCAL_PORT = 3306;

class MySQL{
public:
    MySQL(){
        _conn = mysql_init(nullptr);
    }

    ~MySQL(){
        if (_conn != nullptr){
            mysql_close(_conn);
        }
    }

    bool connect(){
        MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),password.c_str(), dbname.c_str(), MYSQL_LOCAL_PORT, nullptr, 0);
        if (p != nullptr){
            LOG_INFO << "connect mysql sucess!";
            mysql_query(_conn, "set names gbk") ; //  Chinese support
        } else {
            LOG_INFO << "connect mysql fails!";
        }
        return p;
    }

    bool update(std::string sql){
        if (mysql_query(_conn, sql.c_str())){
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << " update fails!";
            return false;
        }
        return true;
    }
    
    MYSQL_RES* query(std::string sql){
        if (mysql_query(_conn, sql.c_str())) {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << " query fails!";
            return nullptr;
        }
        return mysql_use_result(_conn);
    }
    MYSQL* getConnection(){ return _conn;}

private:
    MYSQL *_conn;
};



#endif /* MYSQL_RAII */

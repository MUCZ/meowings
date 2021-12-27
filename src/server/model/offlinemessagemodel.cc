#include "offlinemessagemodel.hh"
#include "mysql_raii.hh"


void OfflineMsgModel::insert(int userid, string&& msg){
   char sql[1024]{0};
   sprintf(sql, "insert into offlinemessage values(%d,'%s')",userid,msg.c_str());

   MySQL mysql;
   if(mysql.connect()){
       mysql.update(sql);
   }
}


void OfflineMsgModel::remove(int userid){
    char sql[1024]{0};
    sprintf(sql, "delete frome offlinemessage where userid=%d",userid);

    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

vector<string> OfflineMsgModel::retrieve(int userid){
    char sql[1024]{0};
    sprintf(sql, "select message from offlinemessage where userid = %d ",userid);

    vector<string> msgs;
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                msgs.emplace_back(row[0]);
            }
            mysql_free_result(res);
            return msgs;
        }
    }
    return msgs;
}
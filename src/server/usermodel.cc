#include "usermodel.hh"
#include "mysql_raii.hh"
#include <iostream>
bool UserModel::insert(User &user){
    char sql[1024]{0};       

    sprintf(sql, "insert into user(name, password,state) values('%s','%s','%s')",
            user.getName().c_str(),user.getPwd().c_str(),user.getState().c_str());


    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            user.setId(mysql_insert_id(mysql.getConnection()));
            LOG_INFO << "registration sucess, name: " << user.getName() << " id: " << user.getId();
            return true;
        }
    }
    return false;
}
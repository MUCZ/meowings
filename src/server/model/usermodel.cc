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

User UserModel::query(int id){ 
    char sql[1024]{0};
    sprintf(sql,"select * from user where id = %d",id);

    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr){
                LOG_INFO << "user found, id = "<<id << " name = " << row[1] ;
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    LOG_INFO << "user not found, id = " << id;
    return User();
}

bool UserModel::updateState(User user){
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = '%d'",user.getState().c_str(),user.getId());

    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "update user state, id = " << user.getId() << " name = "<< user.getName() << " state = "<<user.getState();
            return true;
        }
    }
    return false;

}

void UserModel::resetState(){
    char sql[1024]{"update user set state = 'offline' where state = 'online'"};
    
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "reset users states";
        }
    }
}
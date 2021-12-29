#include "groupmodel.hh"
#include "mysql_raii.hh"

bool GroupModel::createGroup(Group& group){
    char sql[1024]{0};
    sprintf(sql, 
"insert into \
allgroup(groupname,groupdesc) \
values('%s','%s')", 
group.getName().c_str(),group.getDesc().c_str());

    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            group.setId(mysql_insert_id(mysql.getConnection()));
            LOG_INFO << "group created ! name = "<< group.getName() << " id = "<< group.getId();
            return true;
        }
    }
    return false;
}


bool GroupModel::joinGroup(int userid,int groupid, string role){ 
    //  todo : check group exist
    //  fixme : check re-join
    char sql[1024]{0};
    sprintf(sql, "insert into groupuser values(%d,%d,'%s')",
            groupid,userid,role.c_str());
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "id "<<userid <<" join group " << groupid;
            return true;
        }
    }
    return false;
}

vector<Group> GroupModel::queryGroups(int userid){
    char sql[1024]{0};

    // query groups that contain user
    sprintf(sql, 
"select a.id,a.groupname,a.groupdesc \
from \
allgroup a inner join groupuser b \
on \
a.id = b.groupid \
where \
b.userid = %d",userid);

    vector<Group> groupVec;
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
           while( (row = mysql_fetch_row(res)) != nullptr){
               Group group;
               group.setId(atoi(row[0]));
               group.setName(row[1]);
               group.setDesc(row[2]);
               groupVec.emplace_back(group);
           }
           mysql_free_result(res);
        }
    }

    // query user's info
    for(auto& g : groupVec){
        sprintf(sql, 
"select \
a.id, a.name, a.state, b.grouprole \
from \
user a inner join groupuser b \
on \
b.userid = a.id \
where \
b.groupid = %d",g.getId());
        MYSQL_RES* res = mysql.query(sql);
        if(res!= nullptr){
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))!=nullptr ){
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                g.getUsers().emplace_back(user);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}

vector<int> GroupModel::queryGroupUsers(int userid, int groupid){
    char sql[1024]{0};
    sprintf(sql, 
"select userid \
from \
groupuser where groupid = %d", groupid);
    vector<int> idVec;
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
           MYSQL_ROW row;
           while( (row = mysql_fetch_row(res))!= nullptr){
               idVec.emplace_back(atoi(row[0]));
           }
           mysql_free_result(res);
        }
    }
    return idVec;
}
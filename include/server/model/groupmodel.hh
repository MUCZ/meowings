#ifndef GROUPMODEL
#define GROUPMODEL

#include "group.hh"

class GroupModel{

public:
    bool createGroup(Group& group);
    bool joinGroup(int userid, int groupid, string role);
    vector<Group> queryGroups(int userid);
    vector<int> queryGroupUsers(int userid, int groupid);

};

#endif /* GROUPMODEL */

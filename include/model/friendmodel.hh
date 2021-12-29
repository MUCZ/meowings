#ifndef FRIENDMODEL
#define FRIENDMODEL
#include "user.hh"
#include <vector> 

class FriendModel{

public:
    // add friends
    bool insert(int userid, int friendid);

    vector<User> query(int userid);

};

#endif /* FRIENDMODEL */

#ifndef GROUP
#define GROUP
#include "user.hh"
#include <vector>
#include <string>
using namespace std;

class GroupUser : public User{

public: 
    void setRole(string role) { _role = role; }
    inline const string getRole() const  { return _role; }

private:
    string _role;
};

class Group{

public:
    Group(int id = -1, string name = "", string desc = "")
    :_id(id),_name(name),_desc(desc),_users()
    {
    }

    void setId(int id) { _id = id; }
    void setName(string name) { _name = name; }
    void setDesc(string desc) { _desc = desc; }

    const int getId() const { return _id; }
    const string getName() const { return _name; }
    const string getDesc() const { return _desc; }
    vector<GroupUser> &getUsers() { return _users; }

private:
    int _id;
    string _name;
    string _desc;
    vector<GroupUser> _users;
};


#endif /* GROUP */

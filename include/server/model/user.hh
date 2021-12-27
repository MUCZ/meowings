#ifndef USER
#define USER

#include <string>
#include "public.hh"

using namespace std;

class User{
public:
    User(string name="default_name", string pwd="default_pwd", string state=offline, int id = -1)
    : _name(name),_password(pwd),_state(state),_id(id) 
    {
    }

    void setId(int id) { _id = id;}
    void setName(string name) { _name = name;}
    void setPwd(string pwd) { _password = pwd;}
    void setState(string state) { _state = state;}

    int getId() const { return _id;}
    string getName() const { return _name;}
    string getPwd() const { return _password;}
    string getState() const { return _state;}

protected:
    string _name;
    string _password;
    string _state;
    int _id;
};

#endif /* USER */

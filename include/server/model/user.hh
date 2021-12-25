#ifndef USER
#define USER

#include <string>
#include "public.hh"

using namespace std;

class User{
public:
    User(std::string name="default_name", std::string pwd="default_pwd", std::string state=offline, int id = -1)
    : _name(name),_password(pwd),_state(state),_id(id) 
    {
    }

    void setId(int id) { _id = id;}
    void setName(std::string name) { _name = name;}
    void setPwd(std::string pwd) { _password = pwd;}
    void setState(std::string state) { _state = state;}

    int getId() { return _id;}
    std::string getName() { return _name;}
    std::string getPwd() { return _password;}
    std::string getState() { return _state;}

protected:
    std::string _name;
    std::string _password;
    std::string _state;
    int _id;
};

#endif /* USER */

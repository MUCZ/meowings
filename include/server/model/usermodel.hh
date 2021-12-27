#ifndef USERMODEL
#define USERMODEL
#include "user.hh"

class UserModel{
public:
    bool insert(User &user);

    User query(int id);

    bool updateState(User user);

    void resetState();
};

#endif /* USERMODEL */

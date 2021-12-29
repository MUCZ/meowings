#ifndef OFFLINEMESSAGEMODEL
#define OFFLINEMESSAGEMODEL

#include <string>
#include <vector>
using namespace std;

class OfflineMsgModel {

public:
    void insert(int userid, string&& msg);

    bool remove(int userid);

    vector<string> retrieve(int userid);
};

#endif /* OFFLINEMESSAGEMODEL */
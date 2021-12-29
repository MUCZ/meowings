#include "dummyUI.hh"
#include "string_hash_compile_time.hh"


unordered_map<string, string> commandMap = {
    {" | help           ", "show all commands          `help/h`"},
    {" | logout         ", "logout                     `logout/l`"},
    {" | showfriend     ", "show list of friends       `showf/f`"},
    {" | showgroup      ", "show list of groups        `showg/g`"},
    {" | chat           ", "send message to a friend   `chat/c`"},
    {" | addfriend      ", "add friend                 `add/a`"},
    {" | creatgroup     ", "creat a group              `creatg`"},
    {" | joingroup      ", "joint a group              `joing/j`"},
    {" | groupchat      ", "send a chat in a group     `chatg/cg`"}
    };

void mainMenu(){  
    cout << "======================="<<endl;  
    cout << "1. login" << endl;
    cout << "2. register" << endl;
    cout << "3. quit" << endl;
    cout << "======================="<<endl;  

    cout << "choice: ";
    int choice = 0;
    cin >> choice; 
    cin.get(); // read the `\n`
    switch(choice){ 
        case 1:{ // login
            int id = 0;
            char pwd[50]{};
            cout << "userid: " ;
            cin >> id;
            cin.get(); // read the `\n`
            cout << "password: ";
            cin.getline(pwd,50);
            if(g_client->login(id,pwd)){
                online_mainMenu(); 
            } else {
                cout <<"login fails"<<endl;
            }
        } break;
        case 2:{ // register
            char name[50]{};
            char pwd[50]{};
            cout << "username: ";
            cin.getline(name, 50);
            cout << "userpassword: ";
            cin.getline(pwd, 50);
            int id = g_client->reg(name,pwd);
            if(id != 0){
                cout << "register sucesses, id = "<< id <<endl;;
            } else {
            }
        } break;
        case 3:{
            g_client->logout();
            g_client->disconnect();
            cout<<"bye"<<endl;
            exit(0);
        } break;
        default:{
            cin.clear();
            cin.ignore();
            cerr << "invalid input!" << endl;
        } ;
    }
}

void online_mainMenu(){ 
    help_online_mainMenu();
    while(true){
        char buffer[1024]{};
        cout << " -> :" ;
        cout.flush();
        cin.getline(buffer,1024);
        switch(hash_(buffer)){
            // todo add fault tolerance
            case "h"_hash:
            case "help"_hash:{
                help_online_mainMenu();             
            }break;
            case "l"_hash:
            case "out"_hash:
            case "logout"_hash:{
                g_client->logout();
                return;
            }break;
            case "f"_hash:
            case "showf"_hash:{
                g_client->update();
                g_client->showFriends();
            }break;
            case "g"_hash:
            case "showg"_hash:{
                g_client->update();
                g_client->showGroups();
            }break;
            case "c"_hash:
            case "chat"_hash:{
                int friendid{};
                char msgbuffer[1024]{};
                cout << "   -> friendid : ";
                cin >> friendid;
                cin.get();
                cout << "   -> message : ";
                cin.getline(msgbuffer,1024);
                string msg(msgbuffer);
                g_client->oneChat(friendid,msg);
            }break;
            case "a"_hash:
            case "add"_hash:{
                int friendid = 0;
                cout << "   -> friend ID :";
                cin >> friendid;
                cin.get();
                g_client->addFriend(friendid);
            }break;
            case "creatg"_hash:{
                char namebuffer[200]{};
                char descbuffer[400]{};
                cout << "   -> group name : ";
                cin.getline(namebuffer,200);
                cout << "   -> group description : ";
                cin.getline(descbuffer,400);
                string name(namebuffer),desc(descbuffer);
                g_client->createGroup(name,desc);
            }break;
            case "j"_hash:
            case "joing"_hash:{
                int groupid{};
                char msgbuffer[1024]{};
                cout << "   -> groupid : ";
                cin >> groupid;
                cin.get();
                g_client->joinGroup(groupid);
            }break;
            case "cg"_hash :
            case "chatg"_hash:{
                int groupid{};
                char msgbuffer[1024]{};
                cout << "   -> groupid : ";
                cin >> groupid;
                cin.get();
                cout << "   -> message : ";
                cin.getline(msgbuffer,1024);
                string msg(msgbuffer);
                g_client->groupChat(groupid,msg);
            }break;
            default:{
                cin.clear();
                cin.ignore();
                cerr << " -> invalid input!" << endl;
            }
        }
    }
}

void help_online_mainMenu(){
    cout << "========WELCOME========"<<endl;  
    cout << "Usage: "<<endl;
    for(const auto& i : commandMap){
        cout << i.first << " :  " << i.second<<endl;
    }
    cout <<endl;
}
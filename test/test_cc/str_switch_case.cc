#include <string>
#include <iostream>
using namespace std;

typedef uint64_t hash_t;

constexpr hash_t prime = 0x100000001B3ull;  
constexpr hash_t basis = 0xCBF29CE484222325ull;  
hash_t hash_(char const* str){
    hash_t ret{basis};
    while(*str){
        ret ^= *str;
        ret *= prime;
        str++;
    }
    return ret;
}

constexpr hash_t hash_compile_time(char const *str,hash_t last_value = basis){
    return *str? hash_compile_time(str+1,(*str ^ last_value) * prime) : last_value;
}
constexpr unsigned long long  operator "" _hash(char const * p ,size_t){
    return hash_compile_time(p);
}

int main(){
    while(true){
        char buffer[1024]{};
        cout << " -> :";
        cin.getline(buffer,1024);
        switch(hash_(buffer)){
            // todo add fault tolerance
            case "help"_hash:{
                cout<<"help " << string(buffer)<<endl;
            }break;
            case "logout"_hash:{
                cout<<"logout " << string(buffer)<<endl;
            }break;
            case "showfriends"_hash:{
                cout<<"showfriends " << string(buffer)<<endl;
            }break;
            case "showgroup"_hash:{
                cout<<"showgroup " << string(buffer)<<endl;
            }break;
            case "chat"_hash:{
                cout<<"caht " << string(buffer)<<endl;
            }break;
            case "addfriend"_hash:{
                cout<<"addfriend " << string(buffer)<<endl;
            }break;
            case "creategroup"_hash:{
                cout<<"creategroup " << string(buffer)<<endl;
            }break;
            case "joingourp"_hash:{
                cout<<"joingroup " << string(buffer)<<endl;
            }break;
            case "groupchat"_hash:{
                cout<<"groupchat " << string(buffer)<<endl;
            }break;
            default:{
                cerr << "invalid input!" << string(buffer) <<endl;
            }
        }
    }
}
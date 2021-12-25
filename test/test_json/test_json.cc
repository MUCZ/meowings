#include "json.hpp"
#include <iostream>
#include <vector>
#include <map> 
#include <string>

using json = nlohmann::json;
using namespace std;

int main(){

    // 序列化

    // 普通数据序列化
    json js;
    js["id"] = {1,2,3,4,5};
    js["name"] = "zhang san";
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello from liu shuo";

    cout << js <<endl;

    // 容器序列化
    json js_;
    vector<int> vec{1,2,5};
    js_["list"] = vec;

    map<int,string> m{{1,"一"},{2,"二"},{3,"三"}};
    js_["num"] = m;
    cout << js_ <<endl;
    // 显式dump
    cout << js_.dump()<<endl;

    //  反序列化
    cout << endl;
    string  json_str = js.dump();
    cout << "receive: " << json_str<<endl;

    json js2_parsed = json::parse(json_str);
    
    // 单个变量
    auto name = js2_parsed["name"];
    cout << "name: " << name <<endl;

    // 容器
    auto v = js2_parsed["id"];
    cout << "id : ";
    for(const auto& x : v){
        cout << x  << " ";
    }
    cout << endl;

    // 容器
    map<string,string> name_sentence = js2_parsed["msg"];
    for(const auto& x : name_sentence){
        cout << x.first << " : " <<x.second<<endl;
    }
    cout <<endl;
} 
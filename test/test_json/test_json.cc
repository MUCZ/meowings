#include "json.hpp"
#include <iostream>
#include <vector>
#include <map> 
#include <string>
#include <exception>

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

    // 允许空格、回车等
    json js3_parsed = json::parse("{\"msgid\":4,\"id\":   540000  } \n");
    cout << "allow some noise like \\n and space"  << js3_parsed.dump() <<endl;

    // 可以使用find检查键
    cout << "json above has id "<<(js3_parsed.find("id")!=js3_parsed.end() )<< endl;
    cout << "json above has name "<<(js3_parsed.find("name")!=js3_parsed.end()) << endl;

    // 反序列化get为string
    json js_ex ;
    js_ex["msg"] = "message";
    string js_ex_str = js_ex.dump();  
    json js_ex_r = json::parse( js_ex_str);
    cout <<"use [\"tag\"] directly : "<< js_ex_r["msg"] << endl;
    cout <<"use get<string> : "<< js_ex_r["msg"].get<string>()<< endl;



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

    // 多个json : 不行
    try{
        string two_js_str = js_ex.dump();
        two_js_str += js_ex.dump();
        cout << two_js_str <<endl;
        json two_js = json::parse(two_js_str); 
    } catch (exception E){
        cout << E.what()<<endl;
    }


} 
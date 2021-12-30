# meowings
<p align="center">
  <img src="https://github.com/MUCZ/meowings/blob/main/img/structure.png">
</p>

## 说明：

- 一款由C++17开发的支持负载均衡的集群聊天服务器程序，单台机器支持xx台并发连接
- 包括服务端代码和命令行客户端代码
- 功能包括注册账号，添加好友，创建群组，添加群组，单人聊天，群组聊天，离线消息同步等

## 特性/亮点：
- 使用Muduo网络库，基于Reactor模式+多线程实现高性能
- 基于json的应用层协议
- 使用Mysql实现数据持久化
- 基于redis消息队列实现跨服务器通信
- 利用Nginx实现服务器集群TCP负载均衡
- 使用Muduo\Logging异步输出日志到文件

## 用例图：

<p align="center">
  <img src="https://github.com/MUCZ/meowings/blob/main/img/use_case.png">
</p>

## 代码框架：
- 采用分层结构设计，面向接口编程，数据模块、业务模块与网络模块低耦合。

<p align="center">
  <img src="https://github.com/MUCZ/meowings/blob/main/img/module.png">
</p>

## Muduo框架：

<p align="center">
  <img src="https://github.com/MUCZ/meowings/blob/main/img/Muduo.png">
</p>

## 依赖：
    C++17
    CMake
    redis及hiredis 
    Mysql及开发库
    Nginx, 需配置开启TCP负载均衡功能
    nlohmann::json
    muduo

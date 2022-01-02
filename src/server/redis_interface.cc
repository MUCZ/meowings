#include "redis_interface.hh"
#include <assert.h>
#include <iostream>

Redis::Redis()
    : _publish_context(nullptr),_subscribe_context(nullptr)
    {

    }

Redis::~Redis(){
    if(_publish_context){
        redisFree(_publish_context);
    }
    if(_subscribe_context){
        redisFree(_subscribe_context);
    }
}

bool Redis::connect(){ 
    _publish_context = redisConnect(redis_ip.c_str(),redis_port);
    _subscribe_context = redisConnect(redis_ip.c_str(),redis_port);

    if( _publish_context == nullptr || _subscribe_context == nullptr){
        cerr << "connect redis failed! "<<endl;
        return false;
    }

    // a new thread to recivce redis message
    thread t([&](){observer_channel_message();}); 
    t.detach();
    cout << "connect redis server sucess! "<< endl;

    return true;
}

bool Redis::publish(int channel,string message){
    redisReply* reply = static_cast<redisReply*>(redisCommand(_publish_context,
                                    "PUBLISH %d %s",channel, message.c_str()));
    if(!reply){
        cerr <<"redis publish failed"<<endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// to not to block the thread, `subscribe` only send subscription request 
// and does not accept messages
bool Redis::subscribe(int channel){
    // redisCommand = redisAppendCommand + redisBufferWrite + redisGetReply
    // here we execute the first two manually to not call the redisGetReply 
    // which will cause block

    // write the command to local buffer
    if(REDIS_ERR == redisAppendCommand(_subscribe_context,"SUBSCRIBE %d",channel)){
        cerr << "redis subscribe failed!"<<endl;
        return false;
    }

    // push commands in local buffer to redis-server
    int done = 0;
    while(!done){
        if(REDIS_ERR == redisBufferWrite(_subscribe_context,&done)){
            cerr << "redis subscribe failed!"<<endl;
            return false;
        }
    }
    return true;
}

// same idea as subscribe
bool Redis::unsubscribe(int channel){
    // write the command to local buffer
    if(REDIS_ERR == redisAppendCommand(_subscribe_context,"UNSUBSCRIBE %d",channel)){
        cerr << "redis unsubscribe failed!"<<endl;
        return false;
    }

    // push commands in local buffer to redis-server
    int done = 0;
    while(!done){
        if(REDIS_ERR == redisBufferWrite(_subscribe_context,&done)){
            cerr << "redis unsubscribe failed!"<<endl;
            return false;
        }
    }
    return true;
}
void Redis::observer_channel_message(){
    redisReply *reply = nullptr;
    while(REDIS_OK == redisGetReply(_subscribe_context,(void **)&reply)){
        if (reply && reply->element[2]&&reply->element[2]->str){
            assert(_notify_message_handler);
            _notify_message_handler(atoi(reply->element[1]->str),reply->element[2]->str);
        }
        freeReplyObject(reply);
    }
    cerr << "observer_channel_message quit"<<endl;
}

void Redis::init_notify_handler(function<void(int,string)> fn){
    _notify_message_handler = fn;
}
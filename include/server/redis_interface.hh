#ifndef REDIS_INTERFACE
#define REDIS_INTERFACE

#include <hiredis/hiredis.h>
#include <string>
#include <thread>
#include <functional>
using namespace std;

class Redis{
public:
    Redis();
    ~Redis();

    bool connect();

    bool publish(int channel,string message);

    bool subscribe(int channel);

    bool unsubscribe(int channel);

    // receive subsribed messages in another thread
    void observer_channel_message();

    void init_notify_handler(function<void(int,string)> fn);

private:
    // A redisContext object for publishing 
    redisContext *_publish_context;
    // A redisContext object for subscribing
    redisContext *_subscribe_context;
    // fixme : context might be thread-not-safe

    function<void(int,string)> _notify_message_handler;

    inline static const string redis_ip = "127.0.0.1"; // since C++ 17
    inline const static uint16_t redis_port = 6379; // since C++ 17
};
#endif /* REDIS_INTERFACE */

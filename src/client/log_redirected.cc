#include "log_redirected.hh"
#include <memory>

#include <muduo/base/Logging.h>
#include <muduo/base/LogFile.h>


using namespace muduo;
using namespace std;

// redirect log from stdout to a log file: 
unique_ptr<LogFile> g_logFile{};

void outputFunc(const char* msg, int len){ g_logFile->append(msg,len); }
void flushFunc(){ g_logFile->flush(); }

void set_log_output(int argc, char* argv[]){
    char name[256];
    strncpy(name,argv[0], 256); // name of program
    g_logFile = (make_unique<LogFile>(::basename(name),200*1000,true,1,1));
    Logger::setOutput(outputFunc);
    Logger::setFlush(flushFunc);
    LOG_INFO << "LOG REDIRECTED ! ";
    flushFunc();
}
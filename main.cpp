#include "logger.h"
using namespace std;

Logger& logger = Logger::getInstance();

int main() {
    int n=10;
    //logger.set_log_level(log_level::DEBUG);//默认就是DEBUG
    cerr<<"#######################################\n";
    logger.debug("This is a debug message n=%d",n);
    logger.info("This is a info ");
    logger.warn("This is a warning:(%d,%d)",2,3);
    logger.error("This is a error,n=%d");

    logger.set_log_level(log_level::WARN);
    cerr<<"#######set_log_level(log_level::WARN)############\n";
    logger.debug("This is a debug message n=%d",n);
    logger.info("This is a info ");
    logger.warn("This is a warning:(%d,%d)",2,3);
    logger.error("This is a error,n=%d");

    logger.set_log_level(log_level::ERROR);
    cerr<<"#######set_log_level(log_level::ERROR)############\n";
    logger.debug("This is a debug message n=%d",n);\
    logger.info("This is a info ");
    logger.warn("This is a warning:(%d,%d)",2,3);
    logger.error("This is a error,n=%d");
    return 0;
}
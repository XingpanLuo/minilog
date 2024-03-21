#include "logger.h"

int main() {
    Logger& logger = Logger::getInstance();
    logger.log(log_level::DEBUG,"This is a log message %d",2);

    return 0;
}
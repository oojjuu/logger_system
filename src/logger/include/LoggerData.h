#ifndef AGILE_LOGGER_LOGGER_DATA_H
#define AGILE_LOGGER_LOGGER_DATA_H

#include <memory>
#include <string>

#include "LoggerLevel.h"
#include "LoggerBuffer.h"

namespace agile {
namespace logger {
struct LoggerData;

class LoggerHeader;
/**
*@brief class of logger data
*/
struct LoggerData {
    // configure id
    uint32_t confId = 0;
    // log time
    struct tm tm_time;
    // void point
    void* ptr = nullptr;
    // log data
    std::shared_ptr<LoggerBuffer> loggerBuffer = nullptr;
    // logger header
    LoggerHeader* loggerHeader = nullptr;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_DATA_H

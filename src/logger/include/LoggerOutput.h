#ifndef AGILE_LOGGER_LOGGER_OUTPUT_H
#define AGILE_LOGGER_LOGGER_OUTPUT_H

#include "LoggerBuffer.h"

namespace agile {
namespace logger {
/**
*@brief The log output strategy class can be inherited to implement different output strategies
*/
class LoggerOutput {
public:
    LoggerOutput() = default;
    virtual ~LoggerOutput() = default;

    virtual void OnLoggerData(const std::shared_ptr<LoggerBuffer>& loggerBuffer) = 0;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_OUTPUT_H

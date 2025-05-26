#ifndef AGILE_LOGGER_LOGGER_AGILE_H
#define AGILE_LOGGER_LOGGER_AGILE_H

#include <string>

#include "LoggerBuffer.h"
#include "LoggerLevel.h"

namespace agile {
namespace logger {
/**
*@brief Log object class
*/
class Logger final {
public:
    Logger() = delete;
    Logger(uint32_t confId, const std::string& file, int line, const std::string& func, LogLevel level);
    ~Logger();
    LoggerBuffer& get() const;
};
} // namespace logger
} // namespace agile

#endif // AGILE_LOGGER_LOGGER_AGILE_H
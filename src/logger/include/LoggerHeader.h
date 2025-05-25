#ifndef AGILE_LOGGER_LOGGER_HEADER_H
#define AGILE_LOGGER_LOGGER_HEADER_H

#include <memory>
#include <string>
#include <memory>

#include "LoggerData.h"

namespace agile {
namespace logger {
class LoggerHeader {
public:
    LoggerHeader() = default;
    virtual ~LoggerHeader() = default;
    virtual void DoHeader(const std::shared_ptr<LoggerData>&, const std::string&, int,
        const std::string&, LogLevel) { }
};

class LoggerHeaderImpl : public LoggerHeader {
public:
    LoggerHeaderImpl() = default;
    ~LoggerHeaderImpl() = default;
    void DoHeader(const std::shared_ptr<LoggerData>& data, const std::string& file, int line,
        const std::string& func, LogLevel level);
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_HEADER_H

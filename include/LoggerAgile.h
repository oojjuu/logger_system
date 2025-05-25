#ifndef AGILE_LOGGER_LOGGER_AGILE_H
#define AGILE_LOGGER_LOGGER_AGILE_H

#include <string>

#include "LoggerBuffer.h"
#include "LoggerLevel.h"

namespace agile {
namespace logger {
extern void OnLogger(uint32_t confId, const std::string& file, int line, const std::string& func, LogLevel level);
extern void OnRelease();
extern LoggerBuffer& OnGet();
/**
*@brief Log object class
*/
template <uint32_t N>
class Logger final {
public:
    static_assert(N < 30, "N must be less than 30");
    Logger() = delete;
    Logger(const std::string& file, int line, const std::string& func, LogLevel level) 
    {
        OnLogger(N, file, line, func, level);
    }

    ~Logger() 
    { 
        OnRelease();
    }

    LoggerBuffer& get() const
    {
        return OnGet();
    }
};

} // namespace logger
} // namespace agile

#endif // AGILE_LOGGER_LOGGER_AGILE_H
#ifndef AGILE_LOGGER_LOGGER_LEVEL_H
#define AGILE_LOGGER_LOGGER_LEVEL_H

namespace agile {
namespace logger {
/**
*@brief logger level emum class
*/
enum class LogLevel : uint32_t {
    FATAL = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4,
    TRACE = 5
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_LEVEL_H

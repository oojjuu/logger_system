#ifndef AGILE_LOGGER_LOGGER_OUTPUT_TO_CONSOLE_H
#define AGILE_LOGGER_LOGGER_OUTPUT_TO_CONSOLE_H

#include <iostream>
#include "LoggerOutput.h"

namespace agile {
namespace logger {
/*
*@brief The log output interface implements the OnLoggerMessage interface to achieve console output
*/
class LoggerOutputToConsole : public LoggerOutput {
public:
    LoggerOutputToConsole() = default;
    virtual ~LoggerOutputToConsole() = default;
    
    void OnLoggerData(const std::shared_ptr<LoggerBuffer>& loggerBuffer) override
    {
        const std::shared_ptr<std::string>& str = loggerBuffer->data();
        std::cout << *str << std::endl;
    }
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_OUTPUT_TO_CONSOLE_H

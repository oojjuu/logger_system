#ifndef AGILE_LOGGER_LOGGER_OUTPUT_IMPLH
#define AGILE_LOGGER_LOGGER_OUTPUT_IMPLH

#include <chrono>
#include "LoggerData.h"
#include "LoggerOutput.h"

namespace agile {
namespace logger {
class LoggerOutputImpl : public LoggerOutput {
public:
    LoggerOutputImpl() = default;
    virtual ~LoggerOutputImpl() = default;

    virtual void PreviousCheck(const std::shared_ptr<LoggerData>&) { }

	virtual void Run(const std::chrono::steady_clock::time_point&, bool finished = false)
    {
        static_cast<void>(sync);
    }
    
    void OnLoggerData(const std::shared_ptr<LoggerBuffer>& loggerBuffer) override 
    {
        static_cast<void>(loggerBuffer);
    }

    virtual void OnLoggerData(const std::shared_ptr<LoggerData>&) { }
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_OUTPUT_IMPLH

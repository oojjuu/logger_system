#ifndef AGILE_LOGGER_LOGGER_H
#define AGILE_LOGGER_LOGGER_H

#include "LoggerAgile.h"
#include "LoggerConfig.h"
#include "LoggerOutput.h"

namespace agile {
namespace logger {
// The default configure ID
static constexpr uint32_t kDefaultConfigId = 0;

/**
 * @brief Log initialization function. The log module must be initialized before use
 * @param configFilePath Log configuration file path
 * @param fileNameTag Log name tags are customizable
 * @return Whether the function execution is successful
 */
extern bool InitLogger(const std::string& configFilePath, const std::string& fileNameTag = "");

/**
 * @brief The log module release function needs to be called when the program exits to prevent log loss
 */
extern void DestroyLogger();

/**
 * @brief The log output policy function allows for the customization of output policies
 * @param confId Log configuration ID
 * @param output Log output strategy
 * @return Log configuration class
 */
extern const LoggerConfig* SetLoggerOutput(uint32_t confId, std::shared_ptr<LoggerOutput>&& output);
} // namespace logger
} // namespace agile

#define FILE_LINE_FUNC  __FILE__, __LINE__, __FUNCTION__
#define AgileLogger agile::logger::Logger
#define LogConfId agile::logger::kDefaultConfigId
#define LogLevel agile::logger::LogLevel

// The following are log macros, which can also be customized
#define LOG_TRACE AgileLogger<LogConfId>(FILE_LINE_FUNC, LogLevel::TRACE).get()
#define LOG_DEBUG AgileLogger<LogConfId>(FILE_LINE_FUNC, LogLevel::DEBUG).get()
#define LOG_INFO AgileLogger<LogConfId>(FILE_LINE_FUNC, LogLevel::INFO).get()
#define LOG_WARN AgileLogger<LogConfId>(FILE_LINE_FUNC, LogLevel::WARN).get()
#define LOG_ERROR AgileLogger<LogConfId>(FILE_LINE_FUNC, LogLevel::ERROR).get()
#define LOG_FATAL AgileLogger<LogConfId>(FILE_LINE_FUNC, LogLevel::FATAL).get()
#endif // AGILE_LOGGER_LOGGER_H

#ifndef AGILE_LOGGER_LOGGER_DATA_CACHE_H
#define AGILE_LOGGER_LOGGER_DATA_CACHE_H

#include "LoggerConfig.h"
#include "LoggerData.h"
#include "LoggerWriteInterface.h"

namespace agile {
namespace logger {
static constexpr uint32_t gMaxSizeOfCache = 10;
class LoggerDataCache
{
public:
	LoggerDataCache() = default;
	virtual ~LoggerDataCache() = default;

	virtual std::shared_ptr<LoggerData> GetLoggerData(uint32_t index = 0) = 0;
	virtual void AddWaitingData(uint32_t index, const std::shared_ptr<LoggerData> &data,
		LoggerWriteInterface *obj) = 0;
	virtual bool Run(LoggerWriteInterface *) { return false; }

protected:
	std::shared_ptr<LoggerData> CreateLoggerData(const LoggerConfig &config);
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_DATA_CACHE_H

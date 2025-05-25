#ifndef AGILE_LOGGER_LOGGER_OBJECT_H
#define AGILE_LOGGER_LOGGER_OBJECT_H

#include <memory>
#include <string>
#include <list>
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>
#include <mutex>
#include <chrono>

#include "LoggerData.h"
#include "LoggerConfig.h"
#include "LoggerOutputImpl.h"
#include "LoggerDataCache.h"
#include "LoggerWriteInterface.h"

namespace agile {
namespace logger {
class LoggerObject : public LoggerWriteInterface {
public:
	LoggerObject(int confId, const std::string& nameTag, const std::shared_ptr<LoggerDataCache> &cache, bool hasThread);
	LoggerObject() = delete;
	~LoggerObject() = default;

	void SetLoggerOutput(std::shared_ptr<LoggerOutput>&& output) {
		outputs_.emplace_back(output);
	}
	void HandleData(const std::shared_ptr<LoggerData>& loggerData);

	void Write(const std::shared_ptr<LoggerData>& loggerData);

	bool Run(const std::chrono::steady_clock::time_point& curTime, bool sync = false);

	std::shared_ptr<LoggerData> GetLoggerData();

private:
    const LoggerConfig* config_ = nullptr;
	std::list<std::shared_ptr<LoggerOutput>> outputs_;
	std::shared_ptr<LoggerOutputImpl> fileOutput_;
	std::shared_ptr<LoggerDataCache> cache_;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_OBJECT_H

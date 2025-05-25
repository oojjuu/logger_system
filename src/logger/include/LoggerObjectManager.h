#ifndef AGILE_LOGGER_LOGGER_OBJECT_MANAGER_H
#define AGILE_LOGGER_LOGGER_OBJECT_MANAGER_H

#include <vector>
#include <thread>

#include "LoggerObject.h"
#include "LoggerDataCache.h"

namespace agile {
namespace logger {
class LoggerObjectManager {
public:
	static LoggerObjectManager& GetInstance()
	{
		static LoggerObjectManager instance; 
		return instance;
	}
	~LoggerObjectManager();

	bool Init(const std::string& configFilePath, const std::string& fileNameTag);
	void Destroy();
	std::shared_ptr<LoggerData> GetLoggerData(uint32_t confId);
	void Write(const std::shared_ptr<LoggerData>& loggerData);

	const LoggerConfig* SetLoggerOutput(uint32_t confId, std::shared_ptr<LoggerOutput>&& output);

private:
	LoggerObjectManager() = default;

	void Run();
	
private:
	bool inited_ = false;
	bool running_ = false;
	bool isWriteByThread_ = false;
	std::mutex mutex_;
	std::shared_ptr<std::thread> thread_ = nullptr;
	std::vector<std::shared_ptr<LoggerObject>> datas_;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_OBJECT_MANAGER_H

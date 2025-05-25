#ifndef AGILE_LOGGER_LOGGER_DATA_CACHE_IMPL_H
#define AGILE_LOGGER_LOGGER_DATA_CACHE_IMPL_H

#include <vector>
#include <list>
#include <thread>
#include <memory>

#include "LoggerDataCache.h"
#include "MultiThreadList.h"

namespace agile {
namespace logger {
class CoupleListCache
{
public:
	CoupleListCache() = delete;
	CoupleListCache(size_t limitedSize);
	~CoupleListCache() = default;
	
	std::shared_ptr<LoggerData> GetEmptyData();
	void GetAllData(std::list<std::shared_ptr<LoggerData>> &value);

	bool AddWaitingData(const std::shared_ptr<LoggerData> &data);
	void AddIdleData(const std::shared_ptr<LoggerData> &data);

private:
	std::shared_ptr<MultiThreadList<LoggerData>> waitingList_;
	std::shared_ptr<MultiThreadList<LoggerData>> idleList_;
};

class LoggerDataCacheThreads : public LoggerDataCache
{
public:
	LoggerDataCacheThreads() = delete;
	LoggerDataCacheThreads(const LoggerConfig &config);
	~LoggerDataCacheThreads() = default;

	std::shared_ptr<LoggerData> GetLoggerData(uint32_t index);
	void AddWaitingData(uint32_t index, const std::shared_ptr<LoggerData> &data,
		LoggerWriteInterface *obj);
	bool Run(LoggerWriteInterface *obj);

private:
	const LoggerConfig &config_;
	std::vector<std::shared_ptr<CoupleListCache>> caches_;
};

class LoggerDataCacheLocal : public LoggerDataCache
{
	using LoggerDataVector = std::vector<std::shared_ptr<LoggerData>>;
public:
	LoggerDataCacheLocal() = delete;
	LoggerDataCacheLocal(const LoggerConfig &config);
	~LoggerDataCacheLocal() = default;

	std::shared_ptr<LoggerData> GetLoggerData(uint32_t index);
	void AddWaitingData(uint32_t index, const std::shared_ptr<LoggerData> &data,
		LoggerWriteInterface *obj);

private:
	LoggerDataVector* CreateLoggerBuffers();

private:
	const LoggerConfig &config_;
	static std::list<LoggerDataVector> loggerBuffers_;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_DATA_CACHE_IMPL_H

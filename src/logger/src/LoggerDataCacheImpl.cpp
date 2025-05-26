#include "LoggerDataCacheImpl.h"

#include "LoggerHeader.h"
#include "LoggerConfigManager.h"

namespace agile {
namespace logger {
static LoggerHeader gWithoutHeader = LoggerHeader();
static LoggerHeaderImpl gWithHeader = LoggerHeaderImpl();

CoupleListCache::CoupleListCache(size_t limitedSize) :
    waitingList_{std::make_shared<QuickThreadList<LoggerData>>(limitedSize)},
    idleList_{std::make_shared<QuickThreadList<LoggerData>>()}
{
}

std::shared_ptr<LoggerData> CoupleListCache::GetEmptyData()
{
    return idleList_->PopBack();
}

void CoupleListCache::GetAllData(std::list<std::shared_ptr<LoggerData>> &value)
{
    waitingList_->Swap(value);
}

bool CoupleListCache::AddWaitingData(const std::shared_ptr<LoggerData> &data)
{
    return waitingList_->PushBack(data);
}

void CoupleListCache::AddIdleData(const std::shared_ptr<LoggerData> &data)
{
    idleList_->PushBack(data);
}

std::shared_ptr<LoggerData> LoggerDataCache::CreateLoggerData(const LoggerConfig &config)
{
    std::shared_ptr<LoggerData> loggerData = std::make_shared<LoggerData>();
    loggerData->confId = config.config_id;
    loggerData->loggerBuffer = std::make_shared<LoggerBuffer>(config.logger_buffer_size,
        config.logger_buffer_size_limit);
    loggerData->loggerHeader = config.logger_with_header ? &gWithHeader : &gWithoutHeader;
    return loggerData;
}

LoggerDataCacheThreads::LoggerDataCacheThreads(const LoggerConfig &config) : config_{config}
{
    for (uint32_t i = 0; i < gMaxSizeOfCache; i++) {
        caches_.emplace_back(std::make_shared<CoupleListCache>(config.inner_thread_queue_limited));
    }
}

std::shared_ptr<LoggerData> LoggerDataCacheThreads::GetLoggerData(uint32_t index)
{
    std::shared_ptr<LoggerData> data = caches_[index]->GetEmptyData();
    if (data == nullptr) {
        return CreateLoggerData(config_);
    }
    return data;
}

void LoggerDataCacheThreads::AddWaitingData(uint32_t index, const std::shared_ptr<LoggerData> &data,
    LoggerWriteInterface *obj)
{
    if (!caches_[index]->AddWaitingData(data)) {
        obj->Write(data);
    }
}

bool LoggerDataCacheThreads::Run(LoggerWriteInterface *obj)
{
    bool flag = false;
    for (auto &it : caches_) {
        std::list<std::shared_ptr<LoggerData>> values;
        it->GetAllData(values);
        if (!values.empty()) {
            flag = true;
        }
        while (!values.empty()) {
            std::shared_ptr<LoggerData> data = values.front();
            values.pop_front();
            obj->Write(data);
            it->AddIdleData(data);
        }
    }
    return flag;
}

std::list<LoggerDataCacheLocal::LoggerDataVector> LoggerDataCacheLocal::loggerBuffers_;

LoggerDataCacheLocal::LoggerDataCacheLocal(const LoggerConfig &config) : config_{config}
{
}

static std::mutex gLocalMutex;
LoggerDataCacheLocal::LoggerDataVector* LoggerDataCacheLocal::CreateLoggerBuffers()
{
	std::lock_guard<std::mutex> lock(gLocalMutex);
	LoggerDataVector threadLoggerBuffer;
	const std::vector<LoggerConfig>& configs = LoggerConfigManager::GetInstance().configs();
	for (const auto& it : configs) {
		std::shared_ptr<LoggerData> loggerData = CreateLoggerData(it);
		threadLoggerBuffer.emplace_back(std::move(loggerData));
	}
	loggerBuffers_.emplace_back(std::move(threadLoggerBuffer));
	return &loggerBuffers_.back();
}

std::shared_ptr<LoggerData> LoggerDataCacheLocal::GetLoggerData(uint32_t index)
{
    static_cast<void>(index);
	static thread_local LoggerDataVector* localLoggerDataBuffers = nullptr;
	if (localLoggerDataBuffers == nullptr) {
		localLoggerDataBuffers = CreateLoggerBuffers();
	}
	std::shared_ptr<LoggerData> loggerData = (*localLoggerDataBuffers)[config_.config_id];
	time_t timestamp = time(0);
	loggerData->tm_time = *localtime(&timestamp);
	static constexpr uint32_t startYear = 1900;
	loggerData->tm_time.tm_year += startYear;
	loggerData->tm_time.tm_mon++;
    return loggerData;
}

void LoggerDataCacheLocal::AddWaitingData(uint32_t index, const std::shared_ptr<LoggerData> &data,
    LoggerWriteInterface *obj)
{
    static_cast<void>(index);
    obj->Write(data);
}
}
}

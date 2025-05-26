#include "LoggerObject.h"

#include "LoggerConfigManager.h"
#include "LoggerOutputToConsole.h"
#include "LoggerOutputToFile.h"
#include "LoggerUtil.h"

namespace agile {
namespace logger {
LoggerObject::LoggerObject(int confId, const std::string& nameTag,
    const std::shared_ptr<LoggerDataCache> &cache, bool hasThread)
{
    config_ = LoggerConfigManager::GetInstance().GetConfig(confId);
    if (config_->is_output_console) {
        outputs_.emplace_back(std::make_shared<LoggerOutputToConsole>());
    }

    fileOutput_ = std::make_shared<LoggerOutputImpl>();
    if (config_->is_output_file && config_->logger_file_size != 0) {
        if (LoggerUtil::CreateDirectory(config_->logger_dir_path)) {
            if (hasThread) {
                fileOutput_ = std::make_shared<LoggerOutputToFile>(nameTag, config_);
            } else {
                fileOutput_ = std::make_shared<LoggerOutputToFileOneThread>(nameTag, config_);
            }
        }
    }
    cache_ =  cache;
}

bool LoggerObject::Run(const std::chrono::steady_clock::time_point& curTime, bool finished)
{
    fileOutput_->Run(curTime, finished);
    return cache_->Run(this);
}

static thread_local uint32_t localCacheIndex = 0;
static thread_local bool localCacheFlag = false;

void LoggerObject::HandleData(const std::shared_ptr<LoggerData>& loggerData)
{
    cache_->AddWaitingData(localCacheIndex, loggerData, this);
}

void LoggerObject::Write(const std::shared_ptr<LoggerData>& loggerData)
{
    fileOutput_->OnLoggerData(loggerData);
    for (auto& it : outputs_) {
        it->OnLoggerData(loggerData->loggerBuffer);
    }
}

static std::atomic<uint32_t> gCacheIndex{0};
std::shared_ptr<LoggerData> LoggerObject::GetLoggerData()
{
    if (!localCacheFlag) {
        localCacheIndex = (gCacheIndex % gMaxSizeOfCache);
        ++gCacheIndex;
        localCacheFlag = true;
    }
	std::shared_ptr<LoggerData> loggerData = cache_->GetLoggerData(localCacheIndex);
	time_t timestamp = time(0);
	loggerData->tm_time = *localtime(&timestamp);
	static constexpr uint32_t startYear = 1900;
	loggerData->tm_time.tm_year += startYear;
	loggerData->tm_time.tm_mon++;
	fileOutput_->PreviousCheck(loggerData);
	return loggerData;
}
} // namespace logger
} // namespace agile

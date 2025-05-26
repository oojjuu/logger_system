#include "LoggerObjectManager.h"

#include <chrono>
#include <functional>

#include "LoggerConfigManager.h"
#include "LoggerDataCacheImpl.h"

namespace agile {
namespace logger {
static constexpr uint32_t sleepMilliSec = 200;
static constexpr uint32_t sleepMilliSecWithWrite = 10;

LoggerObjectManager::~LoggerObjectManager()
{
	Destroy();
}

bool LoggerObjectManager::Init(const std::string& configFilePath, const std::string& fileNameTag)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (inited_) {
		return true;
	}
	if (!LoggerConfigManager::GetInstance().Init(configFilePath)) {
		return false;
	}
	bool hasThread = false;
	const std::vector<LoggerConfig>& configs = LoggerConfigManager::GetInstance().configs();
	for(auto& it : configs) {
		if (it.has_inner_thread) {
			hasThread = true;
			break;
		}
	}
	for(auto& it : configs) {
		std::shared_ptr<LoggerDataCache> cache = nullptr;
		if (it.has_inner_thread) {
			if (it.wrote_by_inner_thread) {
				isWriteByThread_ = it.wrote_by_inner_thread;
				cache = std::make_shared<LoggerDataCacheThreads>(it);
			} else {
				cache = std::make_shared<LoggerDataCacheLocal>(it);
			}
		} else {
			cache = std::make_shared<LoggerDataCacheLocal>(it);
		}
		datas_.emplace_back(std::make_shared<LoggerObject>(it.config_id, fileNameTag, cache, hasThread));
	}

	inited_ = true;
	if (hasThread) {
		running_ = true;
		thread_ = std::make_shared<std::thread>(std::bind(&LoggerObjectManager::Run, this));
	}
	return true;
}

void LoggerObjectManager::Destroy()
{
	if (running_) {
		running_ = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliSec));
		if (thread_->joinable()) {
			thread_->join();
		}
	} else {
		// Wait for other threads to complete the log writing
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliSec));
	}
	for (size_t i = 0; i < datas_.size(); i++) {
		static_cast<void>(datas_[i]->Run(std::chrono::steady_clock::now(), true));
	}
	datas_.clear();
}

const LoggerConfig* LoggerObjectManager::SetLoggerOutput(uint32_t confId, std::shared_ptr<LoggerOutput>&& output)
{
	const LoggerConfig* conf = LoggerConfigManager::GetInstance().GetConfig(confId);
	if (conf != nullptr) {
		datas_[confId]->SetLoggerOutput(std::move(output));
		return conf;
	}
	return nullptr;
}

void LoggerObjectManager::Run()
{
    while (running_) {
		bool flag = false;
		std::chrono::steady_clock::time_point curTime = std::chrono::steady_clock::now();
		for (size_t i = 0; i < datas_.size(); i++) {
			if (datas_[i]->Run(curTime)) {
				flag = true;
			}
		}
		if (!isWriteByThread_) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliSec));
		} else {
			if (!flag) {
				std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliSecWithWrite));
			}
		}
    }
}

std::shared_ptr<LoggerData> LoggerObjectManager::GetLoggerData(uint32_t confId)
{
	return datas_[confId]->GetLoggerData();
}

void LoggerObjectManager::Write(const std::shared_ptr<LoggerData>& loggerData)
{
	datas_[loggerData->confId]->HandleData(loggerData);
}
} // namespace logger
} // namespace agile

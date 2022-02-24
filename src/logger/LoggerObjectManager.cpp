#include "LoggerObjectManager.h"

#include <assert.h>
#include <iostream>

#include "Logger.h"
#include "LoggerConfigManager.h"

namespace agile {
namespace logger {

////////////////////////////////////////////////////////////////////////////////
bool LoggerObjectManager::Init(const std::string& config_file_path, const std::string& file_name_tag) {
	std::lock_guard<std::mutex> lg(thread_mtx_);
	if (init_) {
		return true;
	}
	
	if (!LoggerConfigManager::GetInstance().Init(config_file_path)) {
		return false;
	}

	const std::vector<LoggerConfig>& configs = LoggerConfigManager::GetInstance().configs();
	for(auto& it : configs) {
		datas_.emplace_back(std::make_shared<LoggerObject>(it.conf_id, file_name_tag));
	}
	
	init_ = true;
	th_run_ = true;
	thread_ = std::make_shared<std::thread>(std::bind(&LoggerObjectManager::Run, this));
	return true;
}

void LoggerObjectManager::Destroy() {
	th_run_ = false;
	datas_.clear();
}

const LoggerConfig* LoggerObjectManager::SetLoggerOutput(uint32_t conf_id, std::shared_ptr<LoggerOutput>&& output) {
	const LoggerConfig* res = LoggerConfigManager::GetInstance().GetConfig(conf_id);
	if (res) {
		datas_[conf_id]->SetLoggerOutput(std::move(output));
	}
	return res;
}

void LoggerObjectManager::Run() {
    while (th_run_) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		std::chrono::steady_clock::time_point cur_time = std::chrono::steady_clock::now();
        for(auto& it : datas_) {
			it->Run(cur_time);
		}
    }
}

LoggerDataVec* LoggerObjectManager::CreateLoggerBuffers() {
	std::lock_guard<std::mutex> lg(thread_mtx_);

	LoggerDataVec thread_logger_buffer;
	const std::vector<LoggerConfig>& configs = LoggerConfigManager::GetInstance().configs();
	for (const auto& it : configs) {
		std::shared_ptr<LoggerData> logger_data = std::make_shared<LoggerData>();
		logger_data->conf_id = it.conf_id;
		logger_data->logger_buffer = std::make_shared<LoggerBuffer>(&it);
		thread_logger_buffer.emplace_back(std::move(logger_data));
	}
	logger_buffers_.emplace_back(std::move(thread_logger_buffer));
	return &logger_buffers_.back();//[logger_buffers_.size() - 1];
}

int LoggerObjectManager::GetLoggerData(uint32_t conf_id, std::shared_ptr<LoggerData>& logger_data, bool valid_level) {
	static thread_local LoggerDataVec* kThreadLoggerDataBuffers = nullptr;
	if (!kThreadLoggerDataBuffers) {
		kThreadLoggerDataBuffers = CreateLoggerBuffers();
	}
	logger_data = (*kThreadLoggerDataBuffers)[conf_id];
	if (!valid_level) {
		return 0;
	}
	
	time_t timestamp = time(0);
	logger_data->tm_time = *localtime(&timestamp);
	logger_data->tm_time.tm_year += 1900;
	++logger_data->tm_time.tm_mon;
	datas_[conf_id]->PreviousCheck(logger_data);

	static __thread int kCurThreadId = 0;
	if (kCurThreadId != 0) {
		return kCurThreadId;
	}
	kCurThreadId = syscall(SYS_gettid);
	return kCurThreadId;
}

} // namespace logger
} // namespace agile
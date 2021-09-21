#include "LoggerAgile.h"

#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread>
#include <iostream>

#include "LoggerManager.h"
#include "LoggerConfigManager.h"

namespace agile {
namespace logger {

static const std::string kLoggerLevelNames[7] =
{
	"[TRACE] ",
	"[DEBUG] ",
	"[INFO] ",
	"[WARN] ",
	"[ERROR] ",
	"[FATAL] ",
    "[SYSTEM] "
};

static constexpr size_t kLoggerHeaderElementMaxSizeLimited = 128;
static constexpr size_t kLoggerDateTypeMaxSizeLimited = 32;

static const std::string kLoggerFileLineSplit = ":";
static const std::string kLoggertagSplitChar = "] ";
static const std::string kLoggerFileLineChar = "\n";

Logger::Logger(uint32_t conf_id, const std::string& tag, const std::string& file, 
						int line, const std::string& func, LogLevel level)
{
	OnLogger(conf_id, tag, file, line, func, level);
}

Logger::Logger(uint32_t conf_id, const std::string& tag, const std::string& file, 
						int line, const std::string& func, LogLevel level, const char* format, ...)
{
	OnLogger(conf_id, tag, file, line, func, level);

	std::shared_ptr<LoggerBuffer>& logger_buffer = logger_data_->logger_buffer; 
	logger_buffer->Reserve(logger_buffer->GetConfig()->logger_buffer_size);

	// format日志内容
    va_list vlist;
    va_start(vlist, format);

	size_t index = logger_buffer->GetWriteIndex();
	std::shared_ptr<std::string>& buffer_str = logger_buffer->Data();
    index += vsnprintf(&buffer_str->at(index), logger_buffer->GetConfig()->logger_buffer_size, format, vlist);
    
	va_end(vlist);

	logger_buffer->SetWriteIndex(index);
}

Logger::~Logger()
{
	*logger_data_->logger_buffer << kLoggerFileLineChar;
	LoggerManager::GetInstance().PushData(std::move(logger_data_));
}


void Logger::OnLogger(uint32_t conf_id, const std::string& tag, const std::string& file, 
						int line, const std::string& func, LogLevel level)
{
	assert(LogLevel::TRACE <= level && level <= LogLevel::SYSTEM);

	static pid_t cur_pid = getpid();
	int cur_tid  = LoggerManager::GetInstance().TlsGetTid(conf_id);

	logger_data_ = LoggerManager::GetInstance().PopData(conf_id);

	logger_data_->conf_id = conf_id;
	logger_data_->tag = tag;

	size_t reserve_size = kLoggerHeaderElementMaxSizeLimited + file.length() + func.length();
	logger_data_->logger_buffer->Reserve(reserve_size);

	size_t index = logger_data_->logger_buffer->GetWriteIndex();
	std::shared_ptr<std::string>& buffer_str = logger_data_->logger_buffer->Data();

	const std::string& level_str = kLoggerLevelNames[(uint8_t)level];
	memcpy(&buffer_str->at(index), level_str.c_str(), level_str.length());
	index += level_str.length();
	
	time_t timestamp = time(0);
	logger_data_->tm_time = *localtime(&timestamp);
	logger_data_->tm_time.tm_year += 1900;
	++logger_data_->tm_time.tm_mon;

    index += snprintf(&buffer_str->at(index), kLoggerDateTypeMaxSizeLimited, "%4d-%02d-%02d %02d:%02d:%02d", 
	logger_data_->tm_time.tm_year, logger_data_->tm_time.tm_mon, logger_data_->tm_time.tm_mday,
	logger_data_->tm_time.tm_hour, logger_data_->tm_time.tm_min, logger_data_->tm_time.tm_sec);

	if (tag.empty())
	{
		index += sprintf(&buffer_str->at(index), " [%d][%d] ", cur_pid, cur_tid);
	}
	else
	{
		index += sprintf(&buffer_str->at(index), " [%d][%d] [", cur_pid, cur_tid);
		
		memcpy(&buffer_str->at(index), tag.c_str(), tag.length());
		index += tag.length();

		memcpy(&buffer_str->at(index), kLoggertagSplitChar.c_str(), kLoggertagSplitChar.length());
		index += kLoggertagSplitChar.length();
	}

	memcpy(&buffer_str->at(index), file.c_str(), file.length());
	index += file.length();

	memcpy(&buffer_str->at(index), kLoggerFileLineSplit.c_str(), kLoggerFileLineSplit.length());
	index += kLoggerFileLineSplit.length();

	memcpy(&buffer_str->at(index), func.c_str(), func.length());
	index += func.length();

	index += sprintf(&buffer_str->at(index), ":%d ", line);

	logger_data_->logger_buffer->SetWriteIndex(index);
}

} // namespace logger
} // namespace agile
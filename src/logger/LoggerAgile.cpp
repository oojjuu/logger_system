#include "LoggerAgile.h"

#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread>
#include <iostream>

#include "LoggerData.h"
#include "LoggerObjectManager.h"
#include "LoggerConfigManager.h"

namespace agile {
namespace logger {

// 日志头部信息内容最大长度限制
static constexpr size_t kLoggerHeaderElementMaxSizeLimited = 128;
// 日期字符最大长度限制
static constexpr size_t kLoggerDateTypeMaxSizeLimited = 32;
// 日志文件名和行号分隔符
static const std::string kLoggerFileLineSplit = ":";
// 日志tag分隔符
static const std::string kLoggertagSplitChar = "] ";
// 日志换行符
static const std::string kLoggerFileLineChar = "\n";

// 日志等级描述字符
static const std::vector<std::string> kLoggerLevelNames = 
{
	"[TRACE] ",
	"[DEBUG] ",
	"[INFO] ",
	"[WARN] ",
	"[ERROR] ",
	"[FATAL] ",
    "[SYSTEM] "
};

static bool kIsEnableLogger = false;

bool InitLogger(const std::string& config_file_path, const std::string& file_name_tag)
{
	kIsEnableLogger = LoggerObjectManager::GetInstance().Init(config_file_path, file_name_tag);
	return kIsEnableLogger;
}

void DestroyLogger()
{
	kIsEnableLogger = false;
	LoggerObjectManager::GetInstance().Destroy();
}

const LoggerConfig* SetLoggerOutput(uint32_t conf_id, std::shared_ptr<LoggerOutput>&& output)
{
	if (!kIsEnableLogger)
	{
		return nullptr;
	}
	return LoggerObjectManager::GetInstance().SetLoggerOutput(conf_id, std::move(output));
}

// logger data缓存
static thread_local std::shared_ptr<LoggerData> kThreadLoggerData = nullptr;
// logger buffer
static thread_local std::shared_ptr<LoggerBuffer> kThreadLoggerBuffer = nullptr;

LoggerBuffer& Logger::Get() const
{
	if (!kIsEnableLogger)
	{
		return *kThreadLoggerBuffer;
	}
	return *kThreadLoggerData->logger_buffer;
}

Logger::Logger(uint32_t conf_id, const std::string& tag, const std::string& file, 
				int line, const std::string& func, LogLevel level)
{
	OnLogger(conf_id, tag, file, line, func, level);
}

Logger::Logger(uint32_t conf_id, const std::string& tag, const std::string& file, 
				int line, const std::string& func, LogLevel level, const char* format, ...)
{
	if (OnLogger(conf_id, tag, file, line, func, level))
	{
		std::shared_ptr<LoggerBuffer>& logger_buffer = kThreadLoggerData->logger_buffer; 
		size_t logger_buffer_size = logger_buffer->GetConfig()->logger_buffer_size;
		logger_buffer->Reserve(logger_buffer_size);

		// format日志内容
		va_list vlist;
		va_start(vlist, format);
		size_t index = vsnprintf(logger_buffer->CurData(), logger_buffer_size, format, vlist);
		va_end(vlist);

		logger_buffer->IncrWriteIndex(index);
		LoggerObjectManager::GetInstance().Write(kThreadLoggerData);
	}
}

Logger::~Logger()
{
	if (kIsEnableLogger && kThreadLoggerData->logger_buffer->GetEnable())
	{
		*kThreadLoggerData->logger_buffer << kLoggerFileLineChar;
		LoggerObjectManager::GetInstance().Write(kThreadLoggerData);
	}
	kThreadLoggerData = nullptr;
}

bool Logger::OnLogger(uint32_t conf_id, const std::string& tag, const std::string& file, 
						int line, const std::string& func, LogLevel level)
{
	if (!kIsEnableLogger)
	{
		if (!kThreadLoggerBuffer)
		{
			kThreadLoggerBuffer = std::make_shared<LoggerBuffer>();
		}
		kThreadLoggerBuffer->SetEnable(false);
		return false;
	}
	assert(LogLevel::TRACE <= level && level <= LogLevel::SYSTEM);

	const LoggerConfig* config = LoggerConfigManager::GetInstance().GetConfig(conf_id);
	bool valid_level = level >= config->level;
	int cur_tid = LoggerObjectManager::GetInstance().GetLoggerData(conf_id, kThreadLoggerData, valid_level);
	if (!valid_level)
	{
		kThreadLoggerData->logger_buffer->SetEnable(false);
		return false;
	}
	kThreadLoggerData->conf_id = conf_id;
	kThreadLoggerData->tag = tag;
	kThreadLoggerData->logger_buffer->SetEnable(true);
	if (!config->logger_with_header)
	{
		return true;
	}
	static pid_t cur_pid = getpid();

	size_t reserve_size = kLoggerHeaderElementMaxSizeLimited + file.length() + func.length();
	kThreadLoggerData->logger_buffer->Reserve(reserve_size);

	size_t index = kThreadLoggerData->logger_buffer->GetWriteIndex();
	std::shared_ptr<std::string>& buffer_str = kThreadLoggerData->logger_buffer->Data();

	const std::string& level_str = kLoggerLevelNames[(uint8_t)level];
	memcpy(&buffer_str->at(index), level_str.c_str(), level_str.length());
	index += level_str.length();

    index += snprintf(&buffer_str->at(index), kLoggerDateTypeMaxSizeLimited, "%4d-%02d-%02d %02d:%02d:%02d", 
	kThreadLoggerData->tm_time.tm_year, kThreadLoggerData->tm_time.tm_mon, kThreadLoggerData->tm_time.tm_mday,
	kThreadLoggerData->tm_time.tm_hour, kThreadLoggerData->tm_time.tm_min, kThreadLoggerData->tm_time.tm_sec);

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

	kThreadLoggerData->logger_buffer->SetWriteIndex(index);
	return true;
}

} // namespace logger
} // namespace agile
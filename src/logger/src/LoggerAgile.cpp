#include "LoggerAgile.h"

#include <atomic>
#include <cstring>

#include "LoggerData.h"
#include "LoggerHeader.h"
#include "LoggerObjectManager.h"
#include "LoggerConfigManager.h"

namespace agile {
namespace logger {
bool InitLogger(const std::string& configFilePath, const std::string& fileNameTag)
{
	return LoggerObjectManager::GetInstance().Init(configFilePath, fileNameTag);
}

static std::atomic<bool> gIsDestroyed{false};
void DestroyLogger()
{	
	gIsDestroyed = true;
	LoggerObjectManager::GetInstance().Destroy();
}

const LoggerConfig* SetLoggerOutput(uint32_t confId, std::shared_ptr<LoggerOutput>&& output)
{
	return LoggerObjectManager::GetInstance().SetLoggerOutput(confId, std::move(output));
}

struct LoggerWrite
{
	LoggerWrite()
	{
		loggerData = std::make_shared<LoggerData>();
		loggerData->loggerBuffer = std::make_shared<LoggerBuffer>(0, 0);
	}
	virtual ~LoggerWrite() = default;
	virtual void Write() { }
	std::shared_ptr<LoggerData> loggerData = nullptr;
};

static const std::string kLoggerFileLineChar = "\n";
struct LoggerWriteImpl : public LoggerWrite
{
	LoggerWriteImpl() : LoggerWrite()
	{
		loggerData = nullptr;
	}
	~LoggerWriteImpl() = default;

	void Write()
	{
		(*loggerData->loggerBuffer) << kLoggerFileLineChar;
		LoggerObjectManager::GetInstance().Write(loggerData);
		loggerData = nullptr;
	}
};

static thread_local LoggerWrite gLoggerWrite;
static thread_local LoggerWriteImpl gLoggerWriteImpl;
static thread_local LoggerWrite* helper = &gLoggerWrite;

static void OnLogger(uint32_t confId, const std::string& file, int line, const std::string& func, LogLevel level)
{
	const LoggerConfig* config = LoggerConfigManager::GetInstance().GetConfig(confId);
	if (config == nullptr || gIsDestroyed) {
		helper = &gLoggerWrite;
		return;
	}
	if (level > config->level) {
		helper = &gLoggerWrite;
		return;
	}
	helper = &gLoggerWriteImpl;
	helper->loggerData = LoggerObjectManager::GetInstance().GetLoggerData(confId);
	helper->loggerData->loggerBuffer->SetWriteIndex(0);
	helper->loggerData->loggerHeader->DoHeader(helper->loggerData, file, line, func, level);
}

Logger::Logger(uint32_t confId, const std::string& file, int line, const std::string& func, LogLevel level) 
{
	OnLogger(confId, file, line, func, level);
}

Logger::~Logger() 
{ 
	helper->Write();
}

LoggerBuffer& Logger::get() const
{
	return *(helper->loggerData->loggerBuffer);
}
} // namespace logger
} // namespace agile
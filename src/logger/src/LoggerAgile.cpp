#include "LoggerAgile.h"

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

void DestroyLogger()
{
	LoggerObjectManager::GetInstance().Destroy();
}

const LoggerConfig* SetLoggerOutput(uint32_t confId, std::shared_ptr<LoggerOutput>&& output)
{
	return LoggerObjectManager::GetInstance().SetLoggerOutput(confId, std::move(output));
}

struct LoggerHelper
{
	LoggerHelper()
	{
		loggerData = std::make_shared<LoggerData>();
		loggerData->loggerBuffer = std::make_shared<LoggerBuffer>(0, 0);
	}
	virtual ~LoggerHelper() = default;
	virtual void Write() { }
	std::shared_ptr<LoggerData> loggerData = nullptr;
};

static const std::string kLoggerFileLineChar = "\n";
struct LoggerHelperImpl : public LoggerHelper
{
	LoggerHelperImpl() : LoggerHelper()
	{
		loggerData = nullptr;
	}
	~LoggerHelperImpl() = default;

	void Write()
	{
		(*loggerData->loggerBuffer) << kLoggerFileLineChar;
		LoggerObjectManager::GetInstance().Write(loggerData);
		loggerData = nullptr;
	}
};

static thread_local LoggerHelper gLoggerHelper;
static thread_local LoggerHelperImpl gLoggerHelperImpl;
static thread_local LoggerHelper* helper = &gLoggerHelper;

void OnLogger(uint32_t confId, const std::string& file, int line, const std::string& func, LogLevel level)
{
	const LoggerConfig* config = LoggerConfigManager::GetInstance().GetConfig(confId);
	if (level > config->level) {
		helper = &gLoggerHelper;
		return;
	}
	helper = &gLoggerHelperImpl;
	helper->loggerData = LoggerObjectManager::GetInstance().GetLoggerData(confId);
	helper->loggerData->loggerBuffer->SetWriteIndex(0);
	helper->loggerData->loggerHeader->DoHeader(helper->loggerData, file, line, func, level);
}

void OnRelease()
{
	helper->Write();
}

LoggerBuffer& OnGet()
{
	return *(helper->loggerData->loggerBuffer);
}
} // namespace logger
} // namespace agile
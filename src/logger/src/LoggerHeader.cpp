#include "LoggerHeader.h"

#include <thread>
#include <cstring>
#include <unistd.h>
#include "LoggerUtil.h"

namespace agile {
namespace logger {
static constexpr size_t kLoggerHeaderFileMaxSizeLimited = 200;
struct LevelNameInfo
{
	std::string name;
	size_t size;
};

static const std::vector<LevelNameInfo> kLoggerLevelNames = {
	{"[FATAL] ", 8},
	{"[ERROR] ", 8},
	{"[WARN] ", 7},
	{"[INFO] ", 7},
	{"[DEBUG] ", 8},
	{"[TRACE] ", 8}
};

static uint64_t GetThreadId()
{
	std::thread::id thisId = std::this_thread::get_id();
	uint64_t *tId = reinterpret_cast<uint64_t*>(&thisId);
	return *tId;
}

static void DateToString(std::string &date, size_t &index, const struct tm &tm_time)
{
    date[index++] = '2';
    date[index++] = '0' + ((tm_time.tm_year / 100) % 10);
    date[index++] = '0' + ((tm_time.tm_year / 10) % 10);
    date[index++] = '0' + (tm_time.tm_year % 10);

    date[index++] = '-';
    date[index++] = '0' + (tm_time.tm_mon / 10);
    date[index++] = '0' + (tm_time.tm_mon % 10);
    
    date[index++] = '-';
    date[index++] = '0' + (tm_time.tm_mday / 10);
    date[index++] = '0' + (tm_time.tm_mday % 10);
    
    date[index++] = ' ';
    date[index++] = '0' + (tm_time.tm_hour / 10);
    date[index++] = '0' + (tm_time.tm_hour % 10);
    
    date[index++] = ':';
    date[index++] = '0' + (tm_time.tm_min / 10);
    date[index++] = '0' + (tm_time.tm_min % 10);

    date[index++] = ':';
    date[index++] = '0' + (tm_time.tm_sec / 10);
    date[index++] = '0' + (tm_time.tm_sec % 10); 
}

void LoggerHeaderImpl::DoHeader(const std::shared_ptr<LoggerData>& data, const std::string& file,
    int line, const std::string& func, LogLevel level)
{
	std::shared_ptr<std::string>& bufferStr = data->loggerBuffer->data();

	const LevelNameInfo& levelInfo = kLoggerLevelNames[(uint8_t)level];
	size_t index = 0;
	memcpy(&bufferStr->at(index), levelInfo.name.c_str(), levelInfo.size);
	index += levelInfo.size;

	DateToString(*bufferStr, index, data->tm_time);
    
    (*bufferStr)[index++] = ' ';

    (*bufferStr)[index++] = '[';
    static pid_t curPid = getpid();
    index += LoggerUtil::NumToString(*bufferStr, index, static_cast<uint64_t>(curPid));
    (*bufferStr)[index++] = ']';

    
    (*bufferStr)[index++] = '[';
    static thread_local uint64_t curTid = GetThreadId();
    index += LoggerUtil::NumToString(*bufferStr, index, curTid);
    (*bufferStr)[index++] = ']';

    (*bufferStr)[index++] = ' ';

	size_t size = file.length();
	if (size > kLoggerHeaderFileMaxSizeLimited) {
		std::string filePath = file.substr(size - kLoggerHeaderFileMaxSizeLimited);
		size = filePath.length();
		memcpy(&bufferStr->at(index), filePath.c_str(), size);
		index += size;
	} else {
		memcpy(&bufferStr->at(index), file.c_str(), size);
		index += size;
	}

	(*bufferStr)[index++] = ':';

	size = func.length();
	memcpy(&bufferStr->at(index), func.c_str(), size);
	index += size;
    
	(*bufferStr)[index++] = ':';

    index += LoggerUtil::NumToString(*bufferStr, index, static_cast<uint64_t>(line));

	(*bufferStr)[index++] = ' ';

	data->loggerBuffer->SetWriteIndex(index); 
}
} // logger
} // agile
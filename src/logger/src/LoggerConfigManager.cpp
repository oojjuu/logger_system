#include "LoggerConfigManager.h"

#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>
#include <algorithm>

namespace agile {
namespace logger {
// Default configuration id
static constexpr uint32_t kDefaultConfigId = 0;

// Default log name
static const std::string kDefaultLoggerName = "agile_logger";

// The size of a single log file is 1024 * 1024 * 100 bytes
static constexpr uint64_t kLoggerFileSize = 104857600;

// The number of log files
static constexpr uint32_t kLoggerFileNumber = 100;

// The row log size is set to 2048 bytes by default
static constexpr uint64_t kLoggerBufferSize = 2048;

// The line log size limit is set to 1024 * 1024 bytes by default
static constexpr uint64_t kLoggerBufferSizeLimit = 1024 * 1024;

// Logs are saved at regular intervals, with the time interval unit being milliseconds
static constexpr uint32_t kLoggerSaveIntervalMSec = 2000;

// Cache queue size limit
static constexpr uint32_t kInnerThreadQueueLimited = 10000;

// String and LogLevel mapping table
static const std::map<std::string, LogLevel> kLoggerLevelMap = {
    {"TRACE", LogLevel::TRACE},
    {"DEBUG", LogLevel::DEBUG},
    {"INFO", LogLevel::INFO},
    {"WARN", LogLevel::WARN},
    {"ERROR", LogLevel::ERROR},
    {"FATAL", LogLevel::FATAL}
};

LoggerConfigManager::LoggerConfigManager()
{
    // configs_.emplace_back(MakeDefaultGetConfig());
}

const LoggerConfig* LoggerConfigManager::GetConfig(uint8_t confId) const
{
    if (static_cast<uint32_t>(confId) >= configs_.size()) {
        return nullptr;
    }
    return &configs_[confId];
}

LoggerConfig LoggerConfigManager::MakeDefaultGetConfig() const
{
    LoggerConfig config;
    config.config_id = kDefaultConfigId;
    config.logger_name = kDefaultLoggerName;
    config.logger_dir_path = "./";
    config.level = LogLevel::INFO;
    config.logger_split_with_date = false;
    config.logger_file_size = kLoggerFileSize;
    config.logger_file_number = kLoggerFileNumber;
    config.logger_buffer_size = kLoggerBufferSize;
    config.logger_buffer_size_limit = kLoggerBufferSizeLimit;
    config.is_output_console = false;
    config.is_output_file = true;
    config.save_real_time = false;
    config.save_interval_msec = kLoggerSaveIntervalMSec;
    config.save_on_logger_num = 0;
    config.logger_with_header = true;
    config.has_inner_thread = true;
    config.wrote_by_inner_thread = false;
    config.inner_thread_queue_limited = kInnerThreadQueueLimited;
    return config;
}

bool LoggerConfigManager::Init(const std::string& configFilePath)
{
    if (inited_) {
        std::cerr << "config is inited already with file:" << configFilePath << std::endl;
        return true;
    }
    inited_ = true;

    std::ifstream fin(configFilePath);
    if (!fin.is_open()) {
        std::cerr << "fail to open file:" << configFilePath << std::endl;
        return false;
    }
    std::string data((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    return ReadFromFileData(data);
}

bool LoggerConfigManager::ReadFromFileData(const std::string &data)
{
    std::string strVal;
    size_t i = 0;
    while (i < data.length()) {
        auto pos = data.find("\n", i);
        if (pos == std::string::npos) {
            strVal += data.substr(i, data.length() - i);
            break;
        }
        bool flag = false;
        for (size_t j = i; j < pos; j++) {
            if (data[j] == ' ') {
                continue;
            }
            flag = ((data[j] == '#') || (data[j] == '/' && j + 1 < pos && data[j + 1] == '/'));
            break;
        }
        if (!flag) {
            strVal += data.substr(i, pos - i + 1);
        }
        i = pos + 1;
    }
    configs_.clear();
    uint32_t confId = 0;
    uint32_t offset = 0;
    static constexpr uint32_t kMaxConfigId = 100;
    while ((confId <= kMaxConfigId) && (ReadConfigData(strVal, confId++, offset))) { 
        // do nothing
    }
    return true;
}

bool LoggerConfigManager::ReadConfigData(const std::string& data, uint32_t confId, uint32_t& offset)
{
    LoggerConfig config = MakeDefaultGetConfig();
    uint32_t maxOffset = offset;
    std::string val;
    if (!GetKeyValue(data, offset, maxOffset, "conf_id", val)) {
        return false;
    }
    if (val != std::to_string(confId)) {
        std::cerr << "fail to get confId for confId must in order expected confId="
            << confId << " but " << val << std::endl;
        assert(false);
        return false;
    }
    config.config_id = confId;

    GetKeyValue(data, offset, maxOffset, "logger_name", config.logger_name);
    GetKeyValue(data, offset, maxOffset, "logger_dir_path", config.logger_dir_path);
    if (IsExistLoggerName(config.logger_dir_path, config.logger_name)) {
        std::cerr << "config logger path exist with confId=" << confId << std::endl;
        assert(false);
        return false;
    }

    val.clear();
    if (GetKeyValue(data, offset, maxOffset, "level", val)) {
        transform(val.begin(), val.end(), val.begin(), ::toupper);
        auto it = kLoggerLevelMap.find(val);
        if (it != kLoggerLevelMap.end()) {
            config.level = it->second;
        }
    }

    AutoGetBoolValue(data, offset, maxOffset, "logger_split_with_date", config.logger_split_with_date);
    AutoGetBoolValue(data, offset, maxOffset, "is_output_console", config.is_output_console);
    AutoGetBoolValue(data, offset, maxOffset, "is_output_file", config.is_output_file);
    AutoGetBoolValue(data, offset, maxOffset, "save_real_time", config.save_real_time);
    AutoGetBoolValue(data, offset, maxOffset, "logger_with_header", config.logger_with_header);
    AutoGetBoolValue(data, offset, maxOffset, "has_inner_thread", config.has_inner_thread);
    AutoGetBoolValue(data, offset, maxOffset, "wrote_by_inner_thread", config.wrote_by_inner_thread);

    AutoGetIntValue(data, offset, maxOffset, "logger_file_size", config.logger_file_size, 32, (uint64_t)1024 * 1024 * 1024 * 10);
    AutoGetIntValue(data, offset, maxOffset, "logger_file_number", config.logger_file_number, 5, 10000);
    AutoGetIntValue(data, offset, maxOffset, "logger_buffer_size", config.logger_buffer_size, 32, 1024 * 1024);
    AutoGetIntValue(data, offset, maxOffset, "logger_buffer_size_limit", config.logger_buffer_size_limit, config.logger_buffer_size * 2, 1024 * 10240);
    AutoGetIntValue(data, offset, maxOffset, "save_interval_msec", config.save_interval_msec, 100, 100000000);
    AutoGetIntValue(data, offset, maxOffset, "save_on_logger_num", config.save_on_logger_num, 10, 100000);
    AutoGetIntValue(data, offset, maxOffset, "inner_thread_queue_limited", config.inner_thread_queue_limited, 10, 100000);

    offset = maxOffset;
    configs_.emplace_back(config);
    return true;
}

bool LoggerConfigManager::IsExistLoggerName(const std::string& dirName, const std::string& loggerName)
{
    for(const auto& it : configs_) {
        if ((it.logger_name == loggerName) && (it.logger_dir_path == dirName)) {
            return true;
        }
    }
    return false;
}

void LoggerConfigManager::AutoGetBoolValue(const std::string& data, uint32_t offset,
    uint32_t& maxOffset, const std::string& key, bool& val)
{
    std::string strVal;
    if (!GetKeyValue(data, offset, maxOffset, key, strVal)) {
        return;
    }
    transform(strVal.begin(), strVal.end(), strVal.begin(), ::tolower);
    if (strVal == "true") {
        val = true;
        return;
    }
    if (strVal == "false") {
        val = false;
        return;
    }
    std::cerr << "use default val for key=" << key << " value=" << strVal <<
        " is not bool type" << std::endl;
}

bool LoggerConfigManager::AutoGetIntValue(const std::string& data, uint32_t offset, uint32_t& maxOffset,
    const std::string& key, uint32_t& val, uint32_t min, uint32_t max)
{
    uint64_t intVal = 0;
    if (AutoGetIntValue(data, offset, maxOffset, key, intVal, (uint64_t)min, (uint64_t)max)) {
        val = static_cast<uint32_t>(intVal);
        return true;
    }
    return false;
}

bool LoggerConfigManager::AutoGetIntValue(const std::string& data, uint32_t offset, uint32_t& maxOffset,
    const std::string& key, uint64_t& val, uint64_t min, uint64_t max)
{
    std::string strVal;
    if (!GetKeyValue(data, offset, maxOffset, key, strVal)) {
        return false;
    }
    const uint32_t intVal = (uint32_t)atol(strVal.c_str());
    if ((min <= intVal) && (intVal <= max)) {
        val = intVal;
        return true;
    } else {
        std::cerr << "use default val for key=" << key << " value=" << intVal <<
            " out of range:(" << min << "," << max << ")" << std::endl;
        return false;
    }
}

bool LoggerConfigManager::GetKeyValue(const std::string& data, uint32_t offset, uint32_t& maxOffset,
    const std::string& key, std::string& val)
{
    const std::string::size_type pos = data.find(key, offset);
    if (pos == data.npos) {
        return false;
    }
    size_t index = (size_t)pos + key.length();
    const size_t len = data.length();
    static constexpr size_t kMaxSizeOfConfigValue = 512;
    const size_t maxIndex = std::min(index + kMaxSizeOfConfigValue, len);

    bool flag = false;
    while (index < maxIndex) {
        if ((' ' == data[index]) || ('\n' == data[index])) {
            index++;
            continue;
        }
        flag = (':' == data[index]);
        break;
    }
    maxOffset = maxOffset < index ? index : maxOffset;
    if (!flag) {
        return false;
    }

    for (size_t j = index + 1; j < len; j++) {
        const char c = data[j];
        if ((c == ' ') || (c == '\n')) {
            val = data.substr(index + 1, j - index - 2);
            maxOffset = maxOffset < j ? j : maxOffset;
            return true;
        }
    }
    return false;
}
} // namespace logger
} // namespace agile
#include "LoggerConfigManager.h"

#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>
#include <algorithm>

namespace agile {
namespace logger {

// 默认配置id
static constexpr uint32_t kDefaultConfigId = 0;

// 默认日志名字
static const std::string kDefaultLoggerName = "agile_logger";

// 单个日志文件大小为1024 * 1024 * 100字节
static constexpr uint64_t kLoggerFileSize = 104857600;

// 日志文件数量
static constexpr uint32_t kLoggerFileNumber = 100;

// 行日志大小限制, 默认为1024 * 1024字节
static constexpr uint64_t kLoggerBufferSizeLimit = 1024 * 1024;

// 定时保存日志, 时间间隔单位毫秒
static constexpr uint32_t kLoggerSaveIntervalMSec = 30000;

// 字符串和LogLevel映射表
static const std::map<std::string, LogLevel> kLoggerLevelMap = {
    {"TRACE", LogLevel::TRACE},
    {"DEBUG", LogLevel::DEBUG},
    {"INFO", LogLevel::INFO},
    {"WARN", LogLevel::WARN},
    {"ERROR", LogLevel::ERROR},
    {"FATAL", LogLevel::FATAL},
    {"SYSTEM", LogLevel::SYSTEM}
};

LoggerConfigManager::LoggerConfigManager() {
    configs_.emplace_back(MakeDefaultConfig());
}

LoggerConfig LoggerConfigManager::MakeDefaultConfig() const {
    LoggerConfig config;
    config.conf_id = kDefaultConfigId;
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
    return config;
}

bool LoggerConfigManager::Init(const std::string& config_file_path) {
    if (init_) {
        std::cerr << "config init already file:" << config_file_path << std::endl;
        return true;
    }
    init_ = true;

    std::ifstream fin(config_file_path);
    if (!fin.is_open()) {
        std::cerr << "fail to open file:" << config_file_path << std::endl;
        return false;
    }
    
    std::string data((std::istreambuf_iterator<char>(fin)), 
                    std::istreambuf_iterator<char>());
    
    std::string str_val;
    size_t i = 0;
    while (i < data.length()) {
        auto pos = data.find("\n", i);
        if (pos == std::string::npos) {
            str_val += data.substr(i, data.length() - i);
            break;
        }
        bool flag = false;
        for (size_t j = i; j < pos; j++) {
            if (data[j] == ' ') {
                continue;
            }
            flag = (data[j] == '#' || (data[j] == '/' && j + 1 < pos && data[j + 1] == '/') );
            break;
        }
        if (!flag) {
            str_val += data.substr(i, pos - i + 1);
        }
        i = pos + 1;
    }
    configs_.clear();
    
    uint32_t conf_id = 0;
    uint32_t offset = 0;
    static constexpr uint32_t kMaxConfigId = 100;
    while (conf_id <= kMaxConfigId && ReadConfigData(str_val, conf_id++, offset)) { }

    return true;
}

bool LoggerConfigManager::ReadConfigData(const std::string& data, uint32_t conf_id, uint32_t& offset) {
    LoggerConfig config = MakeDefaultConfig();

    uint32_t max_offset = offset;
    std::string val;
    if (!GetKeyValue(data, offset, max_offset, "conf_id", val)) {
        return false;
    }

    if (val != std::to_string(conf_id)) {
        std::cerr << "fail to get conf_id for conf_id must in order expected conf_id:" 
                  << conf_id << " but " << val << std::endl;
        assert(false);
        return false;
    }

    config.conf_id = conf_id;
    GetKeyValue(data, offset, max_offset, "logger_name", config.logger_name);
    GetKeyValue(data, offset, max_offset, "logger_dir_path", config.logger_dir_path);
    if (IsExistLoggerName(config.logger_dir_path, config.logger_name)) {
        std::cerr << "config logger path exist with conf_id:" << conf_id << std::endl;
        assert(false);
        return false;
    }

    val.clear();
    if (GetKeyValue(data, offset, max_offset, "level", val)) {
        transform(val.begin(), val.end(), val.begin(), ::toupper);
        auto it = kLoggerLevelMap.find(val);
        if (it != kLoggerLevelMap.end()) {
            config.level = it->second;
        }
    }

    AutoGetBoolValue(data, offset, max_offset, "logger_split_with_date", config.logger_split_with_date);
    AutoGetBoolValue(data, offset, max_offset, "is_output_console", config.is_output_console);
    AutoGetBoolValue(data, offset, max_offset, "is_output_file", config.is_output_file);
    AutoGetBoolValue(data, offset, max_offset, "save_real_time", config.save_real_time);
    AutoGetBoolValue(data, offset, max_offset, "logger_with_header", config.logger_with_header);

    AutoGetIntValue(data, offset, max_offset, "logger_file_size", config.logger_file_size, 32, (uint64_t)1024 * 1024 * 1024 * 10);
    AutoGetIntValue(data, offset, max_offset, "logger_file_number", config.logger_file_number, 5, 10000);
    AutoGetIntValue(data, offset, max_offset, "logger_buffer_size", config.logger_buffer_size, 32, 1024 * 1024);
    AutoGetIntValue(data, offset, max_offset, "logger_buffer_size_limit", config.logger_buffer_size_limit, config.logger_buffer_size * 2, 1024 * 10240);
    AutoGetIntValue(data, offset, max_offset, "save_interval_msec", config.save_interval_msec, 100, 100000000);
    AutoGetIntValue(data, offset, max_offset, "save_on_logger_num", config.save_on_logger_num, 10, 100000);

    offset = max_offset;
    configs_.emplace_back(config);
    return true;
}

bool LoggerConfigManager::IsExistLoggerName(const std::string& dir_name, const std::string& logger_name) {
    for(auto& it : configs_) {
        if (it.logger_name == logger_name && it.logger_dir_path == dir_name) {
            return true;
        }
    }
    return false;
}

void LoggerConfigManager::AutoGetBoolValue(const std::string& data, uint32_t offset, 
                                        uint32_t& max_offset, const std::string& key, bool& val) 
{
    std::string str_val;
    if (!GetKeyValue(data, offset, max_offset, key, str_val)) {
        // std::cerr << "use default val for no key:" << key << " after offset:" << offset << std::endl;
        return;
    }

    transform(str_val.begin(), str_val.end(), str_val.begin(), ::tolower);

    if (str_val == "true" || str_val == "1") {
        val = true;
        return;
    }

    if (str_val == "false" || str_val == "0") {
        val = false;
    }
}

bool LoggerConfigManager::AutoGetIntValue(const std::string& data, uint32_t offset, uint32_t& max_offset, 
                                        const std::string& key, uint32_t& val, uint32_t min, uint32_t max) 
{
    uint64_t val_i = 0;
    bool res = AutoGetIntValue(data, offset, max_offset, key, val_i, (uint64_t)min, (uint64_t)max);
    if (res) {
        val = val_i;
    }
    return res;
}

bool LoggerConfigManager::AutoGetIntValue(const std::string& data, uint32_t offset, uint32_t& max_offset, 
                                        const std::string& key, uint64_t& val, uint64_t min, uint64_t max)
{
    std::string str_val;
    if (!GetKeyValue(data, offset, max_offset, key, str_val)) {
        // std::cerr << "use default val for no key:" << key << " after offset:" << offset << std::endl;
        return false;
    }

    uint32_t int_val = (uint32_t)atol(str_val.c_str());
    if (min <= int_val && int_val <= max) {
        val = int_val;
        return true;
    }
    else {
        std::cerr << "use default val for key:" << key << " val:" << int_val << 
            "out of range:(" << min << "," << max << ")" << std::endl; 
        return false;
    }
}

bool LoggerConfigManager::GetKeyValue(const std::string& data, uint32_t offset, uint32_t& max_offset, 
                                     const std::string& key, std::string& val)
{
    std::string::size_type pos = data.find(key, offset);
    if (pos == data.npos) {
        return false;
    }

    static size_t kMaxSizeOfConfigValue = 512;

    size_t i = (size_t)pos + key.length();
    size_t len = data.length();
    size_t max_i = std::min(i + kMaxSizeOfConfigValue, len);
    bool flag = false;
    char c;
    while (i < max_i) {
        c = data[i];
        if (':' == c) {
            flag = true;
            break;
        }
        if (c == ' ' || c == '\n') {
            i++;
            continue;
        }
        else {
            max_offset = max_offset < i ? i : max_offset;
            return true;
        }
    }

    max_offset = max_offset < i ? i : max_offset;
    if (!flag) {
        return true;
    }

    for (size_t j = i + 1; j < len; j++) {
        c = data[j];
        if (c == ' ' || c == '\n') {
            val = data.substr(i+1, j-i-2);
            max_offset = max_offset < j ? j : max_offset;
            return true;
        }
    }
    return true;
}

} // namespace logger
} // namespace agile
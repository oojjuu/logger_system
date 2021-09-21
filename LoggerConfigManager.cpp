#include "LoggerConfigManager.h"

#include <fstream>
#include <sstream>
#include <streambuf>
#include <iostream>
#include <assert.h>
#include <map>

namespace agile {
namespace logger {

// 默认配置id
static constexpr uint32_t kDefaultConfigId = 0;

// 默认日志名字
static const std::string kDefaultLoggerName = "agile_logger.log";

// 单个日志文件大小为1024 * 1024 * 100字节
static constexpr size_t kLoggerFileSizeLimit = 104857600;

// 日志文件数量
static constexpr uint32_t kLoggerFileNumberLimit = 100;

// 行日志大小, 默认为2048字节
static constexpr uint32_t kLoggerBufferSize = 2048;

// 行日志大小限制, 默认为1024 * 1024字节
static constexpr uint32_t kLoggerBufferSizeLimit = 1024 * 1024;

// 日志数据logger_data缓存数量
static constexpr uint32_t kLoggerCachedSize = 30;

// 如果不是实时保存, 当达到指定条数自动批量保存
static constexpr uint32_t kSaveOnMaxLoggerNum = 20;

// 定时保存日志, 时间间隔单位毫秒
static constexpr uint32_t kLoggerSaveIntervalMSec = 1000;

LoggerConfigManager::LoggerConfigManager()
{
    configs_.push_back(MakeDefaultConfig());
}

LoggerConfig LoggerConfigManager::MakeDefaultConfig()
{
    LoggerConfig config;
    config.conf_id = kDefaultConfigId;
    config.logger_name = kDefaultLoggerName;
    config.logger_dir_path = "./";
    config.level = LogLevel::INFO;
    config.logger_name_with_date = true;
    config.logger_name_strict_with_date = false;
    config.logger_file_size_limit = kLoggerFileSizeLimit;
    config.logger_file_number_limit = kLoggerFileNumberLimit;
    config.logger_buffer_size = kLoggerBufferSize;
    config.logger_buffer_size_limit = kLoggerBufferSizeLimit;
    config.is_single_thread_logger = false;
    config.is_output_console = true;
    config.is_output_file = true;
    config.logger_cached_size = kLoggerCachedSize;
    config.save_in_real_time = true;
    config.save_on_max_logger_num = kSaveOnMaxLoggerNum;
    config.save_interval_msec = kLoggerSaveIntervalMSec;
    return config;
}

bool LoggerConfigManager::Init(const std::string& config_file_path)
{
    if (init_)
    {
        std::cerr << "Config Init Already file:" << config_file_path << std::endl;
        return true;
    }
    init_ = true;

    std::ifstream fin(config_file_path);
    if (!fin.is_open())
    {
        std::cerr << "fail to open file:" << config_file_path << std::endl;
        return false;
    }
    
    std::string data((std::istreambuf_iterator<char>(fin)),
                    std::istreambuf_iterator<char>());
                    /*
    std::stringstream ss;
    ss << fin.rdbuf();
    std::string data(ss.str());*/

    std::cout << "data:" << data << std::endl;

    configs_.clear();

    uint32_t conf_id = 0;
    uint32_t offset = 0;
    bool ret_bool = ReadConfigData(data, conf_id++, offset);
    while (ret_bool && conf_id < 100)
    {
        ret_bool = ReadConfigData(data, conf_id++, offset);
    }
    std::cout << "finish init config" << std::endl;
    return true;
}

bool LoggerConfigManager::GetKeyValue(const std::string& data, uint32_t& offset, const std::string& key, std::string& val)
{
    std::string::size_type pos = data.find(key, offset);
    if (pos == data.npos)
    {
        return false;
    }
    static size_t kMaxSizeOfValue = 512;
    size_t i = (size_t)pos + key.length();
    size_t len = data.length();
    size_t count = 0;
    while (i < len && count < kMaxSizeOfValue)
    {
        ++count;
        char c = data[i];
        if (':' != c)
        {
            if (c == ' ')
            {
                i++;
                continue;
            }
            else
            {
                offset = i;
                return true;
            }
        }

        for (size_t j = i + 1; j < len; j++)
        {
            char cv = data[j];
            if (cv == ' ' || cv == '\n')
            {
                val = data.substr(i+1, j-i-2);
                offset = j;
                return true;
            }
        }
        break;
    }
    offset = i;
    return true;
}

bool LoggerConfigManager::IsExistLoggerName(const std::string& dir_name, const std::string& logger_name)
{
    for(auto& it : configs_)
    {
        if (it.logger_name == logger_name && it.logger_dir_path == dir_name)
        {
            return true;
        }
    }
    return false;
}

static const std::map<std::string, LogLevel> kLoggerLevelMap = {
    {"TRACE", LogLevel::TRACE},
    {"DEBUG", LogLevel::DEBUG},
    {"INFO", LogLevel::INFO},
    {"WARN", LogLevel::WARN},
    {"ERROR", LogLevel::ERROR},
    {"FATAL", LogLevel::FATAL},
    {"SYSTEM", LogLevel::SYSTEM}
};

bool LoggerConfigManager::ReadConfigData(const std::string& data, uint32_t conf_id, uint32_t& offset)
{
    LoggerConfig config = MakeDefaultConfig();

    std::string val;
    if (!GetKeyValue(data, offset, "conf_id", val))
    {
        return false;
    }

    if (val != std::to_string(conf_id))
    {
        std::cerr << "conf_id error conf_id must in order val:" << val << ", conf_id:" << conf_id << std::endl;
        assert(val != std::to_string(conf_id));
        return false;
    }

    GetKeyValue(data, offset, "logger_name", config.logger_name);
    GetKeyValue(data, offset, "logger_dir_path", config.logger_dir_path);
    if (IsExistLoggerName(config.logger_dir_path, config.logger_name))
    {
        std::cerr << "config file path exist with conf_id:" << conf_id << std::endl;
        assert(false);
        return false;
    }

    val.clear();
    if (GetKeyValue(data, offset, "level", val))
    {
        auto it = kLoggerLevelMap.find(val);
        if (it != kLoggerLevelMap.end())
        {
            config.level = it->second;
        }
    }

    AutoGetBoolValue(data, offset, "logger_name_with_date", config.logger_name_with_date);

    AutoGetBoolValue(data, offset, "logger_name_strict_with_date", config.logger_name_strict_with_date);

    AutoGetBoolValue(data, offset, "is_single_thread_logger", config.is_single_thread_logger);
    AutoGetBoolValue(data, offset, "is_output_console", config.is_output_console);
    AutoGetBoolValue(data, offset, "is_output_file", config.is_output_file);
    AutoGetBoolValue(data, offset, "save_in_real_time", config.save_in_real_time);

    AutoGetIntValue(data, offset, "logger_file_size_limit", config.logger_file_size_limit, 0, 0);
    AutoGetIntValue(data, offset, "logger_file_number_limit", config.logger_file_number_limit, 0, 0);
    AutoGetIntValue(data, offset, "logger_buffer_size", config.logger_buffer_size, 0, 0);
    AutoGetIntValue(data, offset, "logger_buffer_size_limit", config.logger_buffer_size_limit, 0, 0);
    AutoGetIntValue(data, offset, "logger_cached_size", config.logger_cached_size, 0, 0);
    AutoGetIntValue(data, offset, "save_on_max_logger_num", config.save_on_max_logger_num, 0, 0);
    AutoGetIntValue(data, offset, "save_interval_msec", config.save_interval_msec, 0, 0);
    configs_.emplace_back(config);
    return true;
}

void LoggerConfigManager::AutoGetBoolValue(const std::string& data, uint32_t& offset, const std::string& key, bool& val)
{
    std::string str_val;
    if (GetKeyValue(data, offset, key, str_val))
    {
        if (str_val == "true")
        {
            val = true;
            return;
        }
        if (str_val == "false")
        {
            val = false;
            return;
        }
    }
}

void LoggerConfigManager::AutoGetIntValue(const std::string& data, uint32_t& offset, const std::string& key, uint32_t& val, uint32_t min, uint32_t max)
{
    std::string str_val;
    if (GetKeyValue(data, offset, key, str_val))
    {
        uint32_t int_val = (uint32_t)atol(str_val.c_str());
        val = int_val;
        if (min <= int_val && int_val <= max)
        {
            val = int_val;
        }
    }
}

} // namespace logger
} // namespace agile
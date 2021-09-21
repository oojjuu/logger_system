#pragma once

#include <assert.h>
#include <vector>

#include "LoggerConfig.h"

namespace agile {
namespace logger {

/**
*@brief 日志配置管理
*/
class LoggerConfigManager
{
public:
    static LoggerConfigManager& GetInstance()
    {
        static LoggerConfigManager instance;
        return instance;
    }

    ~LoggerConfigManager() { }
    
    /**
    *@brief 初始化配置信息
    *@param config_file_path 配置文件路基
    *@return bool
    */
    bool Init(const std::string& config_file_path);

    /**
    *@brief 获取配置信息
    *@return const LoggerConfig*
    */
    const LoggerConfig* GetConfig(uint8_t conf_id) const 
    {
        assert((size_t)conf_id < configs_.size());
        return &configs_[conf_id];
    }

    const std::vector<LoggerConfig>& GetConfigs() const { return configs_; }

private:
    LoggerConfigManager();

    bool ReadConfigData(const std::string& data, uint32_t conf_id, uint32_t& offset);
    
    bool GetKeyValue(const std::string& data, uint32_t& offset, const std::string& key, std::string& val);

    bool IsExistLoggerName(const std::string& dir_name, const std::string& logger_name);

    LoggerConfig MakeDefaultConfig();

    void AutoGetBoolValue(const std::string& data, uint32_t& offset, const std::string& key, bool& val);

    void AutoGetIntValue(const std::string& data, uint32_t& offset, const std::string& key, uint32_t& val, uint32_t min, uint32_t max);

private:
    //
    bool init_ = false;
    // 日志配置信息
    std::vector<LoggerConfig> configs_;
};

} // namespace logger
} // namespace agile
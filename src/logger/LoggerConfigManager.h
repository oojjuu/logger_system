#pragma once

#include <assert.h>
#include <vector>

#include "LoggerConfig.h"

namespace agile {
namespace logger {

/**
*@brief 日志配置管理
*/
class LoggerConfigManager {
public:
    static LoggerConfigManager& GetInstance() {
        static LoggerConfigManager instance;
        return instance;
    }

    ~LoggerConfigManager() = default;
    
    /**
    *@brief 初始化配置信息
    *@param config_file_path 配置文件路径
    *@return bool
    */
    bool Init(const std::string& config_file_path);

    /**
    *@brief 获取配置信息
    *@param conf_id 配置ID
    *@return const LoggerConfig*
    */
    const LoggerConfig* GetConfig(uint8_t conf_id) const {
        // assert((size_t)conf_id < configs_.size());
        if ((size_t)conf_id >= configs_.size()) {
            return nullptr;
        }
        return &configs_[conf_id];
    }

    /**
    *@brief 获取所有配置信息
    *@return 日志配置数组
    */
    const std::vector<LoggerConfig>& configs() const { return configs_; }

private:
    LoggerConfigManager();
    /**
    *@brief 创建一个默认配置数据
    *@return cLoggerConfig
    */
    LoggerConfig MakeDefaultConfig() const;

    /**
    *@brief 读取配置文件数据
    *@param data 配置数据
    *@param conf_id 配置ID
    *@param offset 读取配置数据的偏移量
    *@return bool
    */
    bool ReadConfigData(const std::string& data, uint32_t conf_id, uint32_t& offset);

    /**
    *@brief 通过key值获取配置value
    *@param data 配置数据
    *@param offset 读取配置数据的偏移量
    *@param max_offset 记录读取一份配置的最大偏移量
    *@param key 配置键值
    *@param val 配置键值对应的value
    *@return bool
    */   
    bool GetKeyValue(const std::string& data, uint32_t offset, uint32_t& max_offset, 
                    const std::string& key, std::string& val);

    /**
    *@brief 自动通过key值获取配置bool类型数值, 如果获取不到使用默认值
    *@param data 配置数据
    *@param offset 读取配置数据的偏移量
    *@param max_offset 记录读取一份配置的最大偏移量
    *@param key 配置键值
    *@param val 配置键值对应的value
    */  
    void AutoGetBoolValue(const std::string& data, uint32_t offset, 
                        uint32_t& max_offset, const std::string& key, bool& val);

    /**
    *@brief 自动通过key值获取配置整型数值, 如果获取不到使用默认值
    *@param data 配置数据
    *@param offset 读取配置数据的偏移量
    *@param max_offset 记录读取一份配置的最大偏移量
    *@param key 配置键值
    *@param val 配置键值对应的value
    *@param min 数值下限
    *@param max 数值上限
    */ 
    bool AutoGetIntValue(const std::string& data, uint32_t offset, uint32_t& max_offset, 
                        const std::string& key, uint64_t& val, uint64_t min, uint64_t max);

    bool AutoGetIntValue(const std::string& data, uint32_t offset, uint32_t& max_offset, 
                        const std::string& key, uint32_t& val, uint32_t min, uint32_t max);
    /**
    *@brief 日志文件名是否已经配置过
    *@param dir_name 日志目录路径
    *@param logger_name 日志文件名
    *@return bool
    */
    bool IsExistLoggerName(const std::string& dir_name, const std::string& logger_name);

private:
    // 初始化标志
    bool init_ = false;
    // 日志配置信息
    std::vector<LoggerConfig> configs_;
};

} // namespace logger
} // namespace agile
#pragma once

#include <stdarg.h>
#include <string>

#include "LoggerLevel.h"
#include "LoggerData.h"

namespace agile {
namespace logger {

/**
*@brief 日志对象类
*/
class Logger
{
public:
    Logger() = delete;

    /**
    *@brief Logger 构造函数
    *@param conf_id 配置id
    *@param tag 标签
    *@param file 文件名
    *@param line 行号
    *@param func 函数名
    *@param level 日志等级
    */
    Logger(uint32_t conf_id, const std::string& tag, const std::string& file, 
                int line, const std::string& func, LogLevel level);

    /**
    *@brief Logger 构造函数
    *@param conf_id 配置id
    *@param tag 标签
    *@param file 文件名
    *@param line 行号
    *@param func 函数名
    *@param level 日志等级
    *@param format 格式和输出
    */
    Logger(uint32_t conf_id, const std::string& tag, const std::string& file, 
                int line, const std::string& func, LogLevel level, const char* format, ...);

    virtual ~Logger();

    /**
    *@brief 获取logger buffer
    *@return LoggerBuffer
    */
    LoggerBuffer& Get() const { return *logger_data_->logger_buffer; }

private:
    /**
    *@brief  日志输出函数
    *@param conf_id 配置id
    *@param tag 标签
    *@param file 文件名
    *@param line 行号
    *@param func 函数名
    *@param level 日志等级
    */
    void OnLogger(uint32_t conf_id, const std::string& tag, const std::string& file,
                int line, const std::string& func, LogLevel level);

private:
    // 日志数据智能指针
	std::shared_ptr<LoggerData> logger_data_ = nullptr;
};

} // namespace logger
} // namespace agile
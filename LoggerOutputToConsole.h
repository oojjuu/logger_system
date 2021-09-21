#pragma once

#include <iostream>

#include "LoggerOutput.h"

namespace agile {
namespace logger {
/**
*@brief 日志输出接口，完成OnLoggerMessage接口实现控制台输出
*/
class LoggerOutputToConsole : public LoggerOutput
{
public:
    LoggerOutputToConsole(){ }
    virtual ~LoggerOutputToConsole(){ }

    /**
    *@brief 日志输出回调接口
    *@param logger_data日志内容
    */    
    virtual void OnLoggerData(const std::shared_ptr<LoggerData>& logger_data) override
    {
        size_t size = logger_data->logger_buffer->GetWriteIndex();
        std::shared_ptr<std::string>& str = logger_data->logger_buffer->Data();
        str->resize(size);
        std::cout << *str;
    }

    /**
    *@brief 多条日志输出回调接口, 可以合并日志减少写文件次数
    *@param logger_data_list日志内容列表
    */    
    virtual void OnLoggerDatas(const std::list<std::shared_ptr<LoggerData>>& logger_data_list) override
    {
        for (auto& it : logger_data_list)
        {
            size_t size = it->logger_buffer->GetWriteIndex();
            std::shared_ptr<std::string>& str = it->logger_buffer->Data();
            str->resize(size);
            std::cout << *str;
        }
    }
};

} // namespace logger
} // namespace agile
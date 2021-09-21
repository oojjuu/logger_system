#pragma once

#include <list>

#include "LoggerData.h"

namespace agile {
namespace logger {

/**
 *@brief 日志输出接口，完成OnLoggerMessage接口实现不同的输出(如输出到控制台, 日志文件, 日志收集服务等等)
 */
class LoggerOutput
{
public:
    LoggerOutput(){ }
    virtual ~LoggerOutput(){ }

    /**
     *@brief 日志输出回调接口
     *@param logger_data日志内容
     */    
    virtual void OnLoggerData(const std::shared_ptr<LoggerData>& logger_data) = 0;

    /**
     *@brief 多条日志输出回调接口, 可以合并日志减少写文件次数
     *@param logger_data_list日志内容列表
     */    
    virtual void OnLoggerDatas(const std::list<std::shared_ptr<LoggerData>>& logger_data_list) = 0;
};

} // namespace logger
} // namespace agile
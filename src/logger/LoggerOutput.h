#pragma once

#include <list>
#include "LoggerData.h"

namespace agile {
namespace logger {

/**
*@brief 日志输出接口，完成OnLoggerMessage接口实现不同的输出(如输出到控制台, 日志文件, 日志收集服务等等)
*/
class LoggerOutput {
public:
    LoggerOutput() = default;
    virtual ~LoggerOutput() = default;

    /**
    *@brief 刷新日志
    */ 
    virtual void Flush() { }

    /**
    *@brief 同步日志
    */ 
    virtual void Sync() { }

    /**
    *@brief 定时检查日志文件大小
    */ 
    virtual void CheckFiles() { }

    /**
    *@brief 定时检查磁盘
    */ 
    virtual void CheckDisk() { }

    /**
    *@brief 预先检查日志接口
    *@param logger_data日志
    *@param 返回指针数据(文件操作句柄)
    */
    virtual void* PreviousCheck(std::shared_ptr<LoggerData>& logger_data) { return nullptr; }

    /**
    *@brief 日志输出回调接口
    *@param logger_data日志内容
    */    
    virtual void OnLoggerData(const std::shared_ptr<LoggerData>& logger_data) = 0;
};

} // namespace logger
} // namespace agile
#pragma once

#include <chrono>

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
    *@brief 预先检查日志接口
    *@param logger_data日志
    */
    virtual void PreviousCheck(const std::shared_ptr<LoggerData>& logger_data) { }

	/**
	*@brief 定时执行函数 300毫秒执行一次
	*/
	virtual void Run(const std::chrono::steady_clock::time_point& cur_time) { }

    /**
    *@brief 日志输出回调接口
    *@param logger_data日志内容
    */    
    virtual void OnLoggerData(const std::shared_ptr<LoggerData>& logger_data) = 0;
};

} // namespace logger
} // namespace agile
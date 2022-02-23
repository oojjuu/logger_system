#pragma once

#include <iostream>
#include "LoggerOutput.h"

namespace agile {
namespace logger {
/*
*@brief 日志输出接口，完成OnLoggerMessage接口实现控制台输出
*/
class LoggerOutputToConsole : public LoggerOutput {
public:
    LoggerOutputToConsole() = default;
    virtual ~LoggerOutputToConsole() = default;

    /*
    *@brief 日志输出回调接口
    *@param logger_data日志内容
    */    
    virtual void OnLoggerData(const std::shared_ptr<LoggerData>& logger_data) override {
        std::shared_ptr<std::string>& str = logger_data->logger_buffer->data();
        std::cout << *str << std::endl;
    }
};

} // namespace logger
} // namespace agile
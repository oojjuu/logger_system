#pragma once

namespace agile {
namespace logger {

/**
*@brief 日志等级枚举类
*/
enum LogLevel
{
    TRACE     = 0,// trace日志
    DEBUG     = 1,// debug日志
    INFO      = 2,// info日志
    WARN      = 3,// 告警日志
    ERROR     = 4,// 错误日志
    FATAL     = 5,// fatal日志
    SYSTEM    = 6 // 系统错误日志
};

} // namespace logger
} // namespace agile
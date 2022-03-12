#pragma once

namespace agile {
namespace logger {
    
/**
*@brief 日志等级枚举类
*/
enum LogLevel {
    // trace日志
    TRACE     = 0,
    // debug日志
    DEBUG     = 1,
    // info日志
    INFO      = 2,
    // 告警日志
    WARN      = 3,
    // 错误日志
    ERROR     = 4,
    // fatal日志
    FATAL     = 5,
    // 系统错误日志
    SYSTEM    = 6
};

} // namespace logger
} // namespace agile
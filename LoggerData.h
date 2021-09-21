#pragma once

#include <sys/types.h>
#include <memory>
#include <string>

#include "LoggerBuffer.h"

namespace agile {
namespace logger {

/**
*@brief 日志数据类
*/
struct LoggerData
{
    LoggerData(){ }
    
    // 配置id
    uint32_t conf_id = 0;
    // 标签
    std::string tag;
    // 日志时间
    struct tm tm_time;
    // 日志数据
    std::shared_ptr<LoggerBuffer> logger_buffer = nullptr;
};

} // namespace logger
} // namespace agile
#pragma once

#include <string>

#include "LoggerLevel.h"

namespace agile {
namespace logger {

/**
*@brief 日志配置
*/
struct LoggerConfig
{
    // 配置id, 默认为0
    uint32_t conf_id;

    // 日志文件名, 默认为agile_logger
    std::string logger_name;

    // 日志文件保持目录, 默认为当前运行目录
    std::string logger_dir_path;

    // 日志等级, 默认为INFO
    LogLevel level;

    // 日志文件名是否带上日期, 默认为true
    bool logger_name_with_date;

    // 日志文件是否严格按日期拆分, 如果是那么跨天日志大小没到达logger_size也会拆分, 默认为false
    bool logger_name_strict_with_date;

    // 单个日志文件大小, 单位字节, 默认为1024 * 1024 * 100
    uint32_t logger_file_size_limit;

    // 日志文件数量, 超过自动清理旧日志, 默认为100
    uint32_t logger_file_number_limit;

    // 单行日志大小, 默认为2048字节
    uint32_t logger_buffer_size;

    // 单行日志大小限制, 默认为1024 * 1024字节
    uint32_t logger_buffer_size_limit;

    // 是否是单线程调用日志系统, 默认为false
    bool is_single_thread_logger;

    // 是否输出到控制台, 默认为true
    bool is_output_console;

    // 是否输出到日志文件, 默认为true
    bool is_output_file;

    // 日志数据logger_data缓存数量,默认30, 避免每次都要申请内存
    uint32_t logger_cached_size;
    
    // 是否实时保存日志, 默认为true
    bool save_in_real_time;

    // 如果不是实时保存, 当达到指定条数自动批量保存, 默认值20
    uint32_t save_on_max_logger_num;

    // 定时保存日志, 时间间隔单位毫秒, 默认值1000
    uint32_t save_interval_msec;

};

} // namespace logger
} // namespace agile
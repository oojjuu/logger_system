#ifndef AGILE_LOGGER_LOGGER_CONFIG_H
#define AGILE_LOGGER_LOGGER_CONFIG_H

#include <string>
#include "LoggerLevel.h"

namespace agile {
namespace logger {
/**
*@brief Log configuration file class
*/
struct LoggerConfig {
    // Identity of configure, the default is 0
    uint32_t config_id;

    // The log file name is defaulted to agile_logger
    std::string logger_name;

    // The log file saves the directory, which is the current running directory by default
    std::string logger_dir_path;

    // Logger level, the default value INFO
    LogLevel level;

    // The size of a single log file, in bytes, defaults to 1024 * 1024 * 100
    uint64_t logger_file_size;

    // When the number of log files exceeds 100, old logs will be automatically cleared
    uint32_t logger_file_number;

    // The size of a single log line is 2048 bytes by default
    uint64_t logger_buffer_size;

    // The single-line log size limit is set to 1024 * 1024 bytes by default
    uint64_t logger_buffer_size_limit;

    // whether to output to the console, the default is true
    bool is_output_console;

    // whether to output to the log file, the default is true
    bool is_output_file;
    
    /*
    whether to save logs in real-time, defaults to false, enabling this option is not recommended as it may
    affect performance
    */
    bool save_real_time;

    /* 
    If it is not saved in real time, it will be automatically batch saved when the specified number of items is
    reached. The default value is 0 and it is not enabled. It is not recommended to enable it as it will affect performance 
    */
    uint32_t save_on_logger_num;

    // Logs are saved at regular intervals, with the time interval unit being milliseconds. The default value is 2000
    uint32_t save_interval_msec;

    // Whether the log file is split by date or not, the default is false
    bool logger_split_with_date;
    
    // Whether to save the log header(log level, time, file name, etc.), the default is true
    bool logger_with_header;
    
    // Is there an internal log thread? The default is true
    bool has_inner_thread;

    // Whether the file is written by an internal thread is set to true by default
    bool wrote_by_inner_thread;

    // The upper limit of the internal thread buffer queue size
    uint32_t inner_thread_queue_limited;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_CONFIG_H

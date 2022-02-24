#pragma once

#include <mutex>
#include "LoggerOutput.h"
#include "LoggerConfig.h"
#include "LoggerFileHandlerList.h"

namespace agile {
namespace logger {

/**
*@brief 日志输出接口，完成OnLoggerMessage接口实现日志文件输出
*/
class LoggerOutputToFile : public LoggerOutput {
public:
    LoggerOutputToFile() = delete;
    LoggerOutputToFile(const std::string& tag, const LoggerConfig* config):LoggerOutput(),
                    file_name_tag_(tag),config_(config),write_counter_(0) { }
    virtual ~LoggerOutputToFile();

    /**
    *@brief 预先检查日志接口
    *@param logger_data日志
    */
    virtual void PreviousCheck(const std::shared_ptr<LoggerData>& logger_data) override;

	/**
	*@brief 定时执行函数 300毫秒执行一次
	*/
	virtual void Run(const std::chrono::steady_clock::time_point& cur_time) override;

    /**
    *@brief 日志输出回调接口
    *@param logger_data日志内容
    */    
    virtual void OnLoggerData(const std::shared_ptr<LoggerData>& logger_data) override;

private:
    /**
    *@brief 定时检查日志文件大小
    */ 
    void CheckFiles();

    /**
    *@brief 定时检查磁盘
    */ 
    void CheckDisk();

    /**
    *@brief 获取日志文件名
    *@return string 日志文件名
    */
    std::string GetLoggerFileName();

    /**
    *@brief 创建日志文件
    *@param file_date_time 日期时间
    *@param create_type 1:init 2:file size limit 3:date
    *@return bool 是否成功
    */
    bool CreateLoggerFile(uint32_t file_date_time, int create_type);

    /**
    *@brief 重命名所有日志logger_id
    */
    void RenameAllFilesLoggerId();
    
    /**
    *@brief 检查并轮转删除早期日志文件
    */
    void CheckAndDeleteFileNameMap();

private:
    // 日志ID
    uint32_t cur_log_id_ = 0;
    // disk free size limit
    long long disk_free_size_limit_ = 0;
    // tag
    std::string file_name_tag_;
    // logger config data
    const LoggerConfig* config_ = nullptr;
    // write counter
    std::atomic<uint32_t> write_counter_;
    // lock
    std::mutex mtx_;
    // file handler list
    std::shared_ptr<FileHandlerList> file_handler_list_ = nullptr;
    // file map log_id -> file name
    std::map<uint32_t, std::string> file_name_map_;

    // 定时同步日志时间
	std::chrono::steady_clock::time_point sysc_time_val_;
	// 定时检查日志文件时间
	std::chrono::steady_clock::time_point check_file_time_val_;
    // 定时检查磁盘时间
	std::chrono::steady_clock::time_point check_disk_time_val_;
};

} // namespace logger
} // namespace agile
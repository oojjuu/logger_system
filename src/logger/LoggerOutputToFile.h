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
class LoggerOutputToFile : public LoggerOutput
{
public:
    LoggerOutputToFile() = delete;
    LoggerOutputToFile(const LoggerConfig* config):LoggerOutput(),config_(config),write_counter_(0) {}
    virtual ~LoggerOutputToFile();

    /**
    *@brief 设置文件名标签
    *@param tag 标签
    */
    void SetFileNameTag(const std::string& tag) {file_name_tag_ = tag;}

    /**
    *@brief 预先检查日志接口
    *@param logger_data日志
    *@param 返回指针数据(文件操作句柄)
    */
    virtual void* PreviousCheck(std::shared_ptr<LoggerData>& logger_data) override;

    /**
    *@brief 刷新日志
    */ 
    virtual void Flush() override;

    /**
    *@brief 同步日志
    */ 
    virtual void Sync() override;

    /**
    *@brief 定时检查日志文件大小
    */ 
    virtual void CheckFiles() override;

    /**
    *@brief 定时检查磁盘
    */ 
    virtual void CheckDisk() override;

    /**
    *@brief 日志输出回调接口
    *@param logger_data日志内容
    */    
    virtual void OnLoggerData(const std::shared_ptr<LoggerData>& logger_data) override;

    /**
    *@brief 检查并创建日志目录
    *@param dir_path 日志目录
    */ 
    bool CheckAndCreateLogDir(const std::string& dir_path);

private:
    /**
    *@brief 获取磁盘剩余空间
    *@param path 日志路径
    @return long long磁盘剩余空间,单位:字节
    */  
    long long GetDiskFreeCapacity(const std::string& path);

    /**
    *@brief 获取日志文件列表
    *@param path 日志路径
    *@param fliter 文件过滤
    *@param res 结果文件列表
    *@return bool 是否成功
    */
    bool GetLoggerFiles(const std::string& path, const std::string& fliter, std::vector<std::string>& res);

    /**
    *@brief 重命名文件
    *@param logger_id 日志ID
    *@param logging_file 日志文件名
    *@return string 新的日志文件名
    */
    std::string Rename(uint32_t logger_id, const std::string& logging_file);

    /**
    *@brief 获取日志文件名
    *@return string 日志文件名
    */
    std::string GetLoggerFileName();

    /**
    *@brief 获取日志文件大小
    *@param file_name 日志文件名
    *@return int64_t 日志文件大小
    */
    int64_t GetFileSize(const std::string& file_name);

    /**
    *@brief 通过logger_id获取日志文件名
    *@param logger_id 日志ID
    *@param new_logger_id 新的日志ID
    *@return string 日志文件名
    */
    std::string GetLoggerFileNameByLoggerId(uint32_t logger_id, uint32_t& new_logger_id);

    /**
    *@brief 获取日志序号ID
    *@param files_name 文件名列表
    *@param logger_ids 日志序号ID列表
    *@return uint32_t 最新日志序号ID
    */
    uint32_t GetLoggerOrderId(const std::vector<std::string>& files_name, 
                                                std::vector<int64_t>& logger_ids);

    /**
    *@brief 创建日志文件
    *@param file_date_time 日期时间
    *@param create_type 1:init 2:file size limit 3:date
    *@return bool 是否成功
    */
    bool CreateLoggerFile(uint32_t file_date_time, int create_type);

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
    std::atomic<int> write_counter_;
    // lock
    std::mutex mtx_;
    // file handler list
    std::shared_ptr<FileHandlerList> file_handler_list_ = nullptr;
    // file map log_id -> file name
    std::map<uint32_t, std::string> file_name_map_;
};

} // namespace logger
} // namespace agile
#pragma once

#include "LoggerConfig.h"

namespace agile {
namespace logger {

/**
*@brief 日志util
*/
class LoggerUtil {
public:
    LoggerUtil() = default;
    virtual ~LoggerUtil() = default;

    /**
    *@brief 检查并创建日志目录
    *@param dir_path 日志目录
    */ 
    static bool CheckAndCreateLogDir(const std::string& dir_path);

    /**
    *@brief 获取磁盘剩余空间
    *@param path 日志路径
    @return long long磁盘剩余空间,单位:字节
    */  
    static long long GetDiskFreeCapacity(const std::string& path);

    /**
    *@brief 获取日志文件大小
    *@param file_name 日志文件名
    *@return int64_t 日志文件大小
    */
    static int64_t GetFileSize(const std::string& file_name);

    /**
    *@brief 获取日志文件列表
    *@param path 日志路径
    *@param fliter 文件过滤
    *@param res 结果文件列表
    *@return bool 是否成功
    */
    static bool GetLoggerFiles(const std::string& path, const std::string& fliter, std::vector<std::string>& res);

    /**
    *@brief 重命名文件
    *@param logger_id 日志ID
    *@param logging_file 日志文件名
    *@return string 新的日志文件名
    */
    static std::string RenameLogingToLog(const std::string& logging_file);

    /**
    *@brief 通过logger_id获取日志文件名
    *@param config 日志配置数据
    *@param name_tag 日志tag
    *@param logger_id 日志ID
    *@param new_logger_id 新的日志ID
    *@return string 日志文件名
    */
    static std::string GetLoggerFileNameByLoggerId(const LoggerConfig* config, const std::string& name_tag, 
                                                    uint32_t logger_id, uint32_t& new_logger_id);

    /**
    *@brief 获取日志序号ID
    *@param files_name 文件名列表
    *@param logger_ids 日志序号ID列表
    *@return uint32_t 最新日志序号ID
    */
    static uint32_t GetLoggerOrderIds(const std::vector<std::string>& files_name, 
                                                std::vector<int64_t>& logger_ids);
};

} // namespace logger
} // namespace agile
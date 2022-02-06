#pragma once

#include <sys/types.h>
#include <string>

#include "LoggerData.h"

namespace agile {
namespace logger {

/**
*@brief 日志文件类
*/
class LoggerFileHandler
{
public:
    LoggerFileHandler() = default;
    virtual ~LoggerFileHandler();

    /**
    *@brief 打开文件
    *@param file_path 文件路径
    *@return 是否成功
    */
    bool Open(const std::string& file_path);

    /**
    *@brief 关闭文件
    */
    void Close();

    /**
    *@brief 获取文件大小文件
    *@return 大小文件
    */
    int64_t GetFileSize() const;

    /**
    *@brief 异步写入文件，可能会丢失
    *@param logger_data 写入内容
    *@return 是否成功
    */
    bool AsynWirte(const std::shared_ptr<LoggerData>& logger_data);

    /**
    *@brief 同步写入文件
    *@param logger_data 写入内容
    *@return 是否成功
    */
    bool SyncWrite(const std::shared_ptr<LoggerData>& logger_data);

    /**
    *@brief 刷新日志到缓冲区
    *@return 是否成功
    */
    bool Flush();

    /**
    *@brief 同步文件
    *@return 是否成功
    */
    bool Sync();

    /**
    *@brief 文件句柄是否关闭
    *@return bool
    */
    bool IsClosed() const { return file_handler_ == nullptr; }

private:
    // 文件名
    std::string file_path_;
    // 文件句柄
    FILE* file_handler_ = nullptr;
};

} // namespace logger
} // namespace agile
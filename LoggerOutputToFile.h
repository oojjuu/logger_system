#pragma once

#include "LoggerOutput.h"
#include "LoggerConfig.h"

namespace agile {
namespace logger {
/**
*@brief 日志输出接口，完成OnLoggerMessage接口实现日志文件输出
*/
class LoggerOutputToFile : public LoggerOutput
{
public:
    LoggerOutputToFile(){ }
    LoggerOutputToFile(const LoggerConfig* config):config_(config){ }
    virtual ~LoggerOutputToFile(){ }

    /**
    *@brief 日志输出回调接口
    *@param logger_data日志内容
    */    
    virtual void OnLoggerData(const std::shared_ptr<LoggerData>& logger_data) override;

    /**
    *@brief 多条日志输出回调接口, 可以合并日志减少写文件次数
    *@param logger_data_list日志内容列表
    */    
    virtual void OnLoggerDatas(const std::list<std::shared_ptr<LoggerData>>& logger_data_list) override;

private:
    bool CheckFileName(size_t add_size, tm& cur_tm);

    bool GetCurLogName();

private:
    int cur_log_id_ = 0;
    size_t cur_file_size_ = 0;
    struct tm cur_tm_day_;
    std::string cur_file_name_;
    const LoggerConfig* config_ = nullptr;
};

} // namespace logger
} // namespace agile
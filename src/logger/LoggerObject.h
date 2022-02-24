#pragma once

#include <memory>
#include <string>
#include <list>
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>
#include <mutex>
#include <chrono>
#include "LoggerData.h"
#include "LoggerConfig.h"
#include "LoggerOutput.h"
#include "LoggerFileHandlerList.h"

namespace agile {
namespace logger {

/**
*@brief 日志实例类
*/
class LoggerObject {
public:
	LoggerObject(int conf_id, const std::string& file_name_tag);
	LoggerObject() = delete;
	~LoggerObject() = default;

    /**
    *@brief 预先检查日志接口
    *@param logger_data日志
    */
	void PreviousCheck(const std::shared_ptr<LoggerData>& logger_data);

	/**
	 *@brief 写入日志数据
	 *@param logger_data 日志数据引用
	 */
	void Write(const std::shared_ptr<LoggerData>& logger_data);

	/**
	*@brief 添加日志输出接口
	*@param output 输出对象
	*/
	void SetLoggerOutput(std::shared_ptr<LoggerOutput>&& output) {
		outputs_.emplace_back(output);
	}

	/**
	*@brief 定时执行函数 300毫秒执行一次
	*/
	void Run(const std::chrono::steady_clock::time_point& cur_time);

private:
    // 日志配置信息
    const LoggerConfig* config_ = nullptr;
	// output列表
	std::list<std::shared_ptr<LoggerOutput>> outputs_;
	// 定时同步日志时间
	std::chrono::steady_clock::time_point sysc_time_val_;
	// 定时检查日志时间
	std::chrono::steady_clock::time_point check_time_val_;
};

} // namespace logger
} // namespace agile
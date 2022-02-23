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
    *@param 返回指针数据(文件操作句柄)
    */
	void* PreviousCheck(std::shared_ptr<LoggerData>& logger_data);

    /**
    *@brief 预先日志文件和磁盘空间接口
    *@param cur_time 当前时间
    */
	void CheckFileAndDisk(const std::chrono::steady_clock::time_point& cur_time);

	/**
	*@brief 添加日志输出接口
	*@param output 输出对象
	*/
	void SetLoggerOutput(std::shared_ptr<LoggerOutput>&& output);

	/**
	*@brief 定时刷新保存日志
	*/
	void Flush();

	/**
	*@brief 定时刷新日志写入文件
	*@param cur_time 当前时间
	*@return 是否执行Sync
	*/
	bool Sync(const std::chrono::steady_clock::time_point& cur_time);

	/**
	 *@brief 写入日志数据
	 *@param logger_data 日志数据引用
	 */
	void Write(std::shared_ptr<LoggerData>& logger_data);

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
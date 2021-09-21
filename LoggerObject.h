#pragma once

#include <memory>
#include <string>
#include <list>
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>
#include <mutex>

#include "LoggerData.h"
#include "LoggerBuffer.h"
#include "LoggerConfig.h"
#include "LoggerOutput.h"

namespace agile {
namespace logger {

/**
*@brief 日志实例类
*/
class LoggerObject
{
public:
	LoggerObject(int conf_id);
	
	~LoggerObject(){ }

    void Flush();
	
	/**
	*@brief 获取当前线程ID
	*@return 当前线程ID
	*/
	int TlsGetTid()
	{
		static __thread int kCurThreadId = 0;
		if (kCurThreadId != 0) 
		{
			return kCurThreadId;
		}

		kCurThreadId = syscall(SYS_gettid);
		if (cur_thread_id_ != 0)
		{
			is_mul_threads_ = cur_thread_id_ != kCurThreadId;
		}
		else
		{
			cur_thread_id_ = kCurThreadId;
		}
		return kCurThreadId;
	}

	/**
	 *@brief 从日志池获取空闲日志数据
	 *@return LoggerData智能指针
	 */
	std::shared_ptr<LoggerData> PopData();

	/**
	 *@brief 将日志数据push到日志池
	 *@return data LoggerData智能指针
	 */
	void PushData(std::shared_ptr<LoggerData>&& data);

private:
	LoggerObject();

	/**
	 *@brief 从日志池获取空闲日志数据
	 *@return LoggerData智能指针
	 */
	std::shared_ptr<LoggerData> PopDataWithoutLock();

	/**
	 *@brief 将日志数据push到日志池
	 *@return data LoggerData智能指针
	 */
	void PushDataWithoutLock(std::shared_ptr<LoggerData>&& data);

    void FlushWithoutLock();
	
private:
    // 日志配置信息
    const LoggerConfig* config_ = nullptr;
	// 是否是多线程
    bool is_mul_threads_ = false;
	// 当前线程ID
	int cur_thread_id_ = 0;
	// output列表
	std::list<std::shared_ptr<LoggerOutput>> outputs_;
	// 日志数据列表
	std::list<std::shared_ptr<LoggerData>> datas_;
    // 等待持久化日志列表
    std::list<std::shared_ptr<LoggerData>> waiting_datas_;
    // lock
    std::mutex logger_mtx_;
};

} // namespace logger
} // namespace agile
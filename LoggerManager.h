#pragma once

#include <vector>

#include "LoggerObject.h"

namespace agile {
namespace logger {

/**
*@brief 日志管理类
*/
class LoggerManager
{
public:
	static LoggerManager& GetInstance()
	{ 
		static LoggerManager instance; 
		return instance;
	}
	
	~LoggerManager(){ datas_.clear();}

	void Init();

	void Flush();

	void Flush(uint32_t conf_id);

	/**
	*@brief 获取当前线程ID
	*@param conf_id 配置ID
	*@return 当前线程ID
	*/
	int TlsGetTid(uint32_t conf_id);

	/**
	*@brief 从日志池获取空闲日志数据
	*@param conf_id 配置ID
	*@return LoggerData智能指针
	*/
	std::shared_ptr<LoggerData> PopData(uint32_t conf_id);

	/**
	*@brief 将日志数据push到日志池
	*@return data LoggerData智能指针
	*/
	void PushData(std::shared_ptr<LoggerData>&& data);

private:
	LoggerManager();
	
private:
	std::vector<std::shared_ptr<LoggerObject>> datas_;
};

} // namespace logger
} // namespace agile
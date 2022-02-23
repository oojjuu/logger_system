#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include "LoggerObject.h"

namespace agile {
namespace logger {

typedef std::vector<std::shared_ptr<LoggerData>> LoggerDataVec;
/**
*@brief 日志管理类
*/
class LoggerObjectManager {
public:
	static LoggerObjectManager& GetInstance() { 
		static LoggerObjectManager instance; 
		return instance;
	}
	~LoggerObjectManager() { th_run_ = false; }

	/**
	*@brief 初始化日志object函数
	*@param config_file_path 配置文件路径
	*@return 是否成功
	*/
	bool Init(const std::string& config_file_path, const std::string& file_name_tag);

	/**
	 *@brief 获取日志数据
	 *@param conf_id 日志配置ID
	 *@param logger_data 日志数据引用
	 *@param valid_level 是否是有效等级
	 *@return int 当前线程ID
	 */
	int GetLoggerData(uint32_t conf_id, std::shared_ptr<LoggerData>& logger_data, bool valid_level);

	/**
	 *@brief 写入日志数据
	 *@param logger_data 日志数据引用
	 */
	void Write(std::shared_ptr<LoggerData>& logger_data);

	/**
	*@brief 销毁日志模块
	*/
	void Destroy();

	/**
	*@brief 设置日志输出接口
	*@param conf_id 日志配置ID
	*@param output 输出对象
	*@return 日志配置数据
	*/
	const LoggerConfig* SetLoggerOutput(uint32_t conf_id, std::shared_ptr<LoggerOutput>&& output);

private:
	LoggerObjectManager() = default;

	/**
	*@brief 定时处理日志线程执行函数
	*/
	void Run();

	/**
	*@brief 创建日志buffer缓存
	*@return LoggerDataVec*
	*/
	LoggerDataVec* CreateLoggerBuffers();
	
private:
	// 是否已经初始化
	bool init_ = false;
	// 线程runing
	bool th_run_ = false;
	// lock
	std::mutex thread_mtx_;
	// 定时处理日志线程
	std::shared_ptr<std::thread> thread_ = nullptr;

	// logger buffers
	std::list<LoggerDataVec> logger_buffers_;
	// 日志对象数组
	std::vector<std::shared_ptr<LoggerObject>> datas_;
};

} // namespace logger
} // namespace agile
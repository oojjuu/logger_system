#pragma once

#include "LoggerAgile.h"
#include "LoggerOutput.h"

namespace agile {
namespace logger {

// 默认日志配置ID
static constexpr uint32_t kDefaultConfigId = 0;
// 默认标签
static const std::string kDefaultTag;

/**
*@brief 通过配置文件初始化日志
*@param config_file_path 配置文件路径
*@return bool 是否成功
*/
extern bool InitLogger(const std::string& config_file_path, const std::string& file_name_tag="");

/**
*@brief 销毁日志模块，可以在程序结束时调用
*/
extern void DestroyLogger();

/**
*@brief 设置日志输出接口
*@param conf_id 日志配置ID
*@param output 输出对象
*@return 日志配置数据
*/
extern const LoggerConfig* SetLoggerOutput(uint32_t conf_id, std::shared_ptr<LoggerOutput>&& output);

} // namespace logger
} // namespace agile

// 宏定义
#define AL_FLF  __FILE__, __LINE__, __FUNCTION__

///////////////////////////////////////////////////////////////默认日志///////////////////////////////////////////////////////////////////////////

#define LOG_TRACE   Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::TRACE).Get()
#define LOG_DEBUG   Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::DEBUG).Get()
#define LOG_INFO    Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::INFO).Get()
#define LOG_WARN    Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::WARN).Get()
#define LOG_ERROR   Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::ERROR).Get()
#define LOG_FATAL   Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::FATAL).Get()
#define LOG_SYSTEM  Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::SYSTEM).Get()

#define LOG_TRACE1(format, ...) { Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_DEBUG1(format, ...) { Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_INFO1(format, ...) { Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_WARN1(format, ...) { Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_ERROR1(format, ...) { Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_FATAL1(format, ...) { Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_SYSTEM1(format, ...) { Logger(agile::logger::kDefaultConfigId, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::SYSTEM, format, ##__VA_ARGS__); }

///////////////////////////////////////////////////////////////附带tag参数日志/////////////////////////////////////////////////////////////

#define LOG_TAG_TRACE(tag)   Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::TRACE).Get()
#define LOG_TAG_DEBUG(tag)   Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::DEBUG).Get()
#define LOG_TAG_INFO(tag)    Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::INFO).Get()
#define LOG_TAG_WARN(tag)    Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::WARN).Get()
#define LOG_TAG_ERROR(tag)   Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::ERROR).Get()
#define LOG_TAG_FATAL(tag)   Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::FATAL).Get()
#define LOG_TAG_SYSTEM(tag)  Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::SYSTEM).Get()

#define LOG_TAG_TRACE1(tag, format, ...) { Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_TAG_DEBUG1(tag, format, ...) { Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_TAG_INFO1(tag, format, ...) { Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_TAG_WARN1(tag, format, ...) { Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_TAG_ERROR1(tag, format, ...) { Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_TAG_FATAL1(tag, format, ...) { Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_TAG_SYSTEM1(tag, format, ...) { Logger(agile::logger::kDefaultConfigId, #tag,AL_FLF, agile::logger::LogLevel::SYSTEM, format, ##__VA_ARGS__); }

///////////////////////////////////////////////////////////////附带config_id参数日志/////////////////////////////////////////////////////////////

#define LOG_CID_TRACE(config_id)   Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::TRACE).Get()
#define LOG_CID_DEBUG(config_id)   Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::DEBUG).Get()
#define LOG_CID_INFO(config_id)    Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::INFO).Get()
#define LOG_CID_WARN(config_id)    Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::WARN).Get()
#define LOG_CID_ERROR(config_id)   Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::ERROR).Get()
#define LOG_CID_FATAL(config_id)   Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::FATAL).Get()
#define LOG_CID_SYSTEM(config_id)  Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::SYSTEM).Get()

#define LOG_CID_TRACE1(config_id, format, ...) { Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_CID_DEBUG1(config_id, format, ...) { Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_CID_INFO1(config_id, format, ...) { Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_CID_WARN1(config_id, format, ...) { Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_CID_ERROR1(config_id, format, ...) { Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_CID_FATAL1(config_id, format, ...) { Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_CID_SYSTEM1(config_id, format, ...) { Logger(config_id, agile::logger::kDefaultTag,AL_FLF, agile::logger::LogLevel::SYSTEM, format, ##__VA_ARGS__); }

///////////////////////////////////////////////////////////////附带config_id, tag参数日志/////////////////////////////////////////////////////////////

#define LOG_CT_TRACE(config_id, tag)   Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::TRACE).Get()
#define LOG_CT_DEBUG(config_id, tag)   Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::DEBUG).Get()
#define LOG_CT_INFO(config_id, tag)    Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::INFO).Get()
#define LOG_CT_WARN(config_id, tag)    Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::WARN).Get()
#define LOG_CT_ERROR(config_id, tag)   Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::ERROR).Get()
#define LOG_CT_FATAL(config_id, tag)   Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::FATAL).Get()
#define LOG_CT_SYSTEM(config_id, tag)  Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::SYSTEM).Get()

#define LOG_CT_TRACE1(config_id, tag, format, ...) { Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_CT_DEBUG1(config_id, tag, format, ...) { Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_CT_INFO1(config_id, tag, format, ...) { Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_CT_WARN1(config_id, tag, format, ...) { Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_CT_ERROR1(config_id, tag, format, ...) { Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_CT_FATAL1(config_id, tag, format, ...) { Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_CT_SYSTEM1(config_id, tag, format, ...) { Logger(config_id, #tag,AL_FLF, agile::logger::LogLevel::SYSTEM, format, ##__VA_ARGS__); }
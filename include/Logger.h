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
*@param file_name_tag 文件名标签，可以使用进程号
*@return bool 是否成功
*/
extern bool InitLogger(const std::string& config_file_path, const std::string& file_name_tag = "");

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
#define AgileLogger agile::logger::Logger
#define AgileCID agile::logger::kDefaultConfigId
#define AgileTag agile::logger::kDefaultTag
#define ALLevel agile::logger::LogLevel

///////////////////////////////////////////////////////////////默认日志///////////////////////////////////////////////////////////////////////////
#define LOG_TRACE   AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::TRACE).get()
#define LOG_DEBUG   AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::DEBUG).get()
#define LOG_INFO    AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::INFO).get()
#define LOG_WARN    AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::WARN).get()
#define LOG_ERROR   AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::ERROR).get()
#define LOG_FATAL   AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::FATAL).get()
#define LOG_SYSTEM  AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::SYSTEM).get()

#define LOG_TRACE_F(format, ...) { AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_DEBUG_F(format, ...) { AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_INFO_F(format, ...) { AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_WARN_F(format, ...) { AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_ERROR_F(format, ...) { AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_FATAL_F(format, ...) { AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_SYSTEM_F(format, ...) { AgileLogger(AgileCID, AgileTag, AL_FLF, ALLevel::SYSTEM, format, ##__VA_ARGS__); }

///////////////////////////////////////////////////////////////附带tag参数日志/////////////////////////////////////////////////////////////
#define LOG_TRACE_TAG(tag)   AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::TRACE).get()
#define LOG_DEBUG_TAG(tag)   AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::DEBUG).get()
#define LOG_INFO_TAG(tag)    AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::INFO).get()
#define LOG_WARN_TAG(tag)    AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::WARN).get()
#define LOG_ERROR_TAG(tag)   AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::ERROR).get()
#define LOG_FATAL_TAG(tag)   AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::FATAL).get()
#define LOG_SYSTEM_TAG(tag)  AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::SYSTEM).get()

#define LOG_TRACE_TAG_F(tag, format, ...) { AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_DEBUG_TAG_F(tag, format, ...) { AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_INFO_TAG_F(tag, format, ...) { AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_WARN_TAG_F(tag, format, ...) { AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_ERROR_TAG_F(tag, format, ...) { AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_FATAL_TAG_F(tag, format, ...) { AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_SYSTEM_TAG_F(tag, format, ...) { AgileLogger(AgileCID, #tag, AL_FLF, ALLevel::SYSTEM, format, ##__VA_ARGS__); }

///////////////////////////////////////////////////////////////附带conf_id参数日志/////////////////////////////////////////////////////////////
#define LOG_TRACE_CID(conf_id)   AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::TRACE).get()
#define LOG_DEBUG_CID(conf_id)   AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::DEBUG).get()
#define LOG_INFO_CID(conf_id)    AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::INFO).get()
#define LOG_WARN_CID(conf_id)    AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::WARN).get()
#define LOG_ERROR_CID(conf_id)   AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::ERROR).get()
#define LOG_FATAL_CID(conf_id)   AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::FATAL).get()
#define LOG_SYSTEM_CID(conf_id)  AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::SYSTEM).get()

#define LOG_TRACE_CID_F(conf_id, format, ...) { AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_DEBUG_CID_F(conf_id, format, ...) { AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_INFO_CID_F(conf_id, format, ...) { AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_WARN_CID_F(conf_id, format, ...) { AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_ERROR_CID_F(conf_id, format, ...) { AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_FATAL_CID_F(conf_id, format, ...) { AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_SYSTEM_CID_F(conf_id, format, ...) { AgileLogger(conf_id, AgileTag, AL_FLF, ALLevel::SYSTEM, format, ##__VA_ARGS__); }

///////////////////////////////////////////////////////////////附带conf_id, tag参数日志/////////////////////////////////////////////////////////////
#define LOG_TRACE_CT(conf_id, tag)   AgileLogger(conf_id, #tag, AL_FLF, ALLevel::TRACE).get()
#define LOG_DEBUG_CT(conf_id, tag)   AgileLogger(conf_id, #tag, AL_FLF, ALLevel::DEBUG).get()
#define LOG_INFO_CT(conf_id, tag)    AgileLogger(conf_id, #tag, AL_FLF, ALLevel::INFO).get()
#define LOG_WARN_CT(conf_id, tag)    AgileLogger(conf_id, #tag, AL_FLF, ALLevel::WARN).get()
#define LOG_ERROR_CT(conf_id, tag)   AgileLogger(conf_id, #tag, AL_FLF, ALLevel::ERROR).get()
#define LOG_FATAL_CT(conf_id, tag)   AgileLogger(conf_id, #tag, AL_FLF, ALLevel::FATAL).get()
#define LOG_SYSTEM_CT(conf_id, tag)  AgileLogger(conf_id, #tag, AL_FLF, ALLevel::SYSTEM).get()

#define LOG_TRACE_CT_F(conf_id, tag, format, ...) { AgileLogger(conf_id, #tag, AL_FLF, ALLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_DEBUG_CT_F(conf_id, tag, format, ...) { AgileLogger(conf_id, #tag, AL_FLF, ALLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_INFO_CT_F(conf_id, tag, format, ...) { AgileLogger(conf_id, #tag, AL_FLF, ALLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_WARN_CT_F(conf_id, tag, format, ...) { AgileLogger(conf_id, #tag, AL_FLF, ALLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_ERROR_CT_F(conf_id, tag, format, ...) { AgileLogger(conf_id, #tag, AL_FLF, ALLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_FATAL_CT_F(conf_id, tag, format, ...) { AgileLogger(conf_id, #tag, AL_FLF, ALLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_SYSTEM_CT_F(conf_id, tag, format, ...) { AgileLogger(conf_id, #tag, AL_FLF, ALLevel::SYSTEM, format, ##__VA_ARGS__); }
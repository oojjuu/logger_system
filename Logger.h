#pragma once

#include "LoggerAgile.h"

namespace agile {
namespace logger {

// 默认配置ID
static constexpr uint32_t kDCid = 0;
// 默认标签
static const std::string kDTag;

extern bool InitLogger(const std::string& config_file_path);

#define FLF  __FILE__, __LINE__, __FUNCTION__

#define LOG_TRACE   Logger(kDCid, kDTag, FLF, LogLevel::TRACE).Get()
#define LOG_DEBUG   Logger(kDCid, kDTag, FLF, LogLevel::DEBUG).Get()
#define LOG_INFO    Logger(kDCid, kDTag, FLF, LogLevel::INFO).Get()
#define LOG_WARN    Logger(kDCid, kDTag, FLF, LogLevel::WARN).Get()
#define LOG_ERROR   Logger(kDCid, kDTag, FLF, LogLevel::ERROR).Get()
#define LOG_FATAL   Logger(kDCid, kDTag, FLF, LogLevel::FATAL).Get()
#define LOG_SYSTEM  Logger(kDCid, kDTag, FLF, LogLevel::SYSTEM).Get()

#define LOG_TRACE1(format, ...) { Logger(kDCid, kDTag, FLF, LogLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_DEBUG1(format, ...) { Logger(kDCid, kDTag, FLF, LogLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_INFO1(format, ...) { Logger(kDCid, kDTag, FLF, LogLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_WARN1(format, ...) { Logger(kDCid, kDTag, FLF, LogLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_ERROR1(format, ...) { Logger(kDCid, kDTag, FLF, LogLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_FATAL1(format, ...) { Logger(kDCid, kDTag, FLF, LogLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_SYSTEM1(format, ...) { Logger(kDCid, kDTag, FLF, LogLevel::SYSTEM, format, ##__VA_ARGS__); }

//////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG_TAG_TRACE(tag)   Logger(kDCid, #tag, FLF, LogLevel::TRACE).Get()
#define LOG_TAG_DEBUG(tag)   Logger(kDCid, #tag, FLF, LogLevel::DEBUG).Get()
#define LOG_TAG_INFO(tag)    Logger(kDCid, #tag, FLF, LogLevel::INFO).Get()
#define LOG_TAG_WARN(tag)    Logger(kDCid, #tag, FLF, LogLevel::WARN).Get()
#define LOG_TAG_ERROR(tag)   Logger(kDCid, #tag, FLF, LogLevel::ERROR).Get()
#define LOG_TAG_FATAL(tag)   Logger(kDCid, #tag, FLF, LogLevel::FATAL).Get()
#define LOG_TAG_SYSTEM(tag)  Logger(kDCid, #tag, FLF, LogLevel::SYSTEM).Get()

#define LOG_TAG_TRACE1(tag, format, ...) { Logger(kDCid, #tag, FLF, LogLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_TAG_DEBUG1(tag, format, ...) { Logger(kDCid, #tag, FLF, LogLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_TAG_INFO1(tag, format, ...) { Logger(kDCid, #tag, FLF, LogLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_TAG_WARN1(tag, format, ...) { Logger(kDCid, #tag, FLF, LogLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_TAG_ERROR1(tag, format, ...) { Logger(kDCid, #tag, FLF, LogLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_TAG_FATAL1(tag, format, ...) { Logger(kDCid, #tag, FLF, LogLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_TAG_SYSTEM1(tag, format, ...) { Logger(kDCid, #tag, FLF, LogLevel::SYSTEM, format, ##__VA_ARGS__); }

//////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG_CID_TRACE(cid)   Logger(cid, kDTag, FLF, LogLevel::TRACE).Get()
#define LOG_CID_DEBUG(cid)   Logger(cid, kDTag, FLF, LogLevel::DEBUG).Get()
#define LOG_CID_INFO(cid)    Logger(cid, kDTag, FLF, LogLevel::INFO).Get()
#define LOG_CID_WARN(cid)    Logger(cid, kDTag, FLF, LogLevel::WARN).Get()
#define LOG_CID_ERROR(cid)   Logger(cid, kDTag, FLF, LogLevel::ERROR).Get()
#define LOG_CID_FATAL(cid)   Logger(cid, kDTag, FLF, LogLevel::FATAL).Get()
#define LOG_CID_SYSTEM(cid)  Logger(cid, kDTag, FLF, LogLevel::SYSTEM).Get()

#define LOG_CID_TRACE1(cid, format, ...) { Logger(cid, kDTag, FLF, LogLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_CID_DEBUG1(cid, format, ...) { Logger(cid, kDTag, FLF, LogLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_CID_INFO1(cid, format, ...) { Logger(cid, kDTag, FLF, LogLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_CID_WARN1(cid, format, ...) { Logger(cid, kDTag, FLF, LogLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_CID_ERROR1(cid, format, ...) { Logger(cid, kDTag, FLF, LogLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_CID_FATAL1(cid, format, ...) { Logger(cid, kDTag, FLF, LogLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_CID_SYSTEM1(cid, format, ...) { Logger(cid, kDTag, FLF, LogLevel::SYSTEM, format, ##__VA_ARGS__); }

//////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG_CT_TRACE(cid, tag)   Logger(cid, #tag, FLF, LogLevel::TRACE).Get()
#define LOG_CT_DEBUG(cid, tag)   Logger(cid, #tag, FLF, LogLevel::DEBUG).Get()
#define LOG_CT_INFO(cid, tag)    Logger(cid, #tag, FLF, LogLevel::INFO).Get()
#define LOG_CT_WARN(cid, tag)    Logger(cid, #tag, FLF, LogLevel::WARN).Get()
#define LOG_CT_ERROR(cid, tag)   Logger(cid, #tag, FLF, LogLevel::ERROR).Get()
#define LOG_CT_FATAL(cid, tag)   Logger(cid, #tag, FLF, LogLevel::FATAL).Get()
#define LOG_CT_SYSTEM(cid, tag)  Logger(cid, #tag, FLF, LogLevel::SYSTEM).Get()

#define LOG_CT_TRACE1(cid, tag, format, ...) { Logger(cid, #tag, FLF, LogLevel::TRACE, format, ##__VA_ARGS__); }
#define LOG_CT_DEBUG1(cid, tag, format, ...) { Logger(cid, #tag, FLF, LogLevel::DEBUG, format, ##__VA_ARGS__); }
#define LOG_CT_INFO1(cid, tag, format, ...) { Logger(cid, #tag, FLF, LogLevel::INFO, format, ##__VA_ARGS__); }
#define LOG_CT_WARN1(cid, tag, format, ...) { Logger(cid, #tag, FLF, LogLevel::WARN, format, ##__VA_ARGS__); }
#define LOG_CT_ERROR1(cid, tag, format, ...) { Logger(cid, #tag, FLF, LogLevel::ERROR, format, ##__VA_ARGS__); }
#define LOG_CT_FATAL1(cid, tag, format, ...) { Logger(cid, #tag, FLF, LogLevel::FATAL, format, ##__VA_ARGS__); }
#define LOG_CT_SYSTEM1(cid, tag, format, ...) { Logger(cid, #tag, FLF, LogLevel::SYSTEM, format, ##__VA_ARGS__); }

} // namespace logger
} // namespace agile
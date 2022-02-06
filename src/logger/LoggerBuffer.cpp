#include "LoggerBuffer.h"

#include <cstring>
#include <iostream>
#include "LoggerConfigManager.h"

namespace agile {
namespace logger {

const std::string LoggerBuffer::kStlEmptyString = "{}";
const std::string LoggerBuffer::kStlEmptySplitString = ",";
const std::string LoggerBuffer::kStlMapEmptySplitString = ":";

const std::string LoggerBuffer::kBracketLeftString = "{";
const std::string LoggerBuffer::kBracketRightString = "}";

LoggerBuffer::LoggerBuffer(const LoggerConfig* config)
{ 
    config_ = config;
    logger_str_ = std::make_shared<std::string>();
    logger_str_->resize(config_->logger_buffer_size);
}

bool LoggerBuffer::Reserve(size_t data_size)
{
    size_t cur_size = data_size + write_index_;
    if (cur_size <= logger_str_->size())
    {
        return true;
    }

    if (cur_size > config_->logger_buffer_size_limit)
    {
        std::cerr << "logger length out of rang limit_size:" << 
                config_->logger_buffer_size_limit << ", cur_size:" << cur_size << std::endl;
        return false;
    }

    size_t capacity_size = logger_str_->capacity();
    if (cur_size < capacity_size)
    {
        logger_str_->resize(capacity_size);
        return true;
    }
    
    cur_size += cur_size;
    if (cur_size > config_->logger_buffer_size_limit)
    {
        logger_str_->resize(config_->logger_buffer_size_limit);
    }
    else
    {
        logger_str_->resize(cur_size);
    }
    return true;
}

LoggerBuffer& LoggerBuffer::operator << (bool val)
{
    if (!enable_)
    {
        return *this;
    }
    static const std::string kLoggerBoolString[2] = {"true", "false"};
    *this << kLoggerBoolString[val];
    return *this;
}

static const std::string kDPrintFmt = "%d";
static const std::string kLDPrintFmt = "%ld";
static const std::string kLLDPrintFmt = "%lld";

static const std::string kUPrintFmt = "%u";
static const std::string kLUPrintFmt = "%lu";
static const std::string kLLUPrintFmt = "%llu";

static const std::string kFPrintFmt = "%f";

LoggerBuffer& LoggerBuffer::operator << (int8_t val)
{
    WriteWithFmt(val, kDPrintFmt);
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (uint8_t val)
{
    WriteWithFmt(val, kUPrintFmt);
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (int16_t val)
{
    WriteWithFmt(val, kDPrintFmt);
    return *this;
}    
    
LoggerBuffer& LoggerBuffer::operator << (uint16_t val)
{
    WriteWithFmt(val, kUPrintFmt);
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (int32_t val)
{
    WriteWithFmt(val, kDPrintFmt);
    return *this;
}
    
LoggerBuffer& LoggerBuffer::operator << (uint32_t val)
{
    WriteWithFmt(val, kUPrintFmt);
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (int64_t val)
{
    WriteWithFmt(val, kLDPrintFmt);
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (uint64_t val)
{
    WriteWithFmt(val, kLUPrintFmt);
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (float val)
{
    WriteWithFmt(val, kFPrintFmt);
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (double val)
{
    WriteWithFmt(val, kFPrintFmt);
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (const char* val) 
{ 
    if (!enable_)
    {
        return *this;
    }
    size_t len = strlen(val);
    if (Reserve(len))
    {
        memcpy(&logger_str_->at(write_index_), val, len);
        write_index_ += len;
    }
    return *this; 
}

LoggerBuffer& LoggerBuffer::operator << (const std::string& val)
{
    if (!enable_)
    {
        return *this;
    }
    size_t len = val.length();
    if (Reserve(len))
    {
        memcpy(&logger_str_->at(write_index_), val.c_str(), len);
        write_index_ += len;
    }
    return *this;
}

} // namespace logger
} // namespace agile
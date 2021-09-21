#include "LoggerBufferImp.h"

#include <cstring>
#include <iostream>

#include "LoggerConfigManager.h"

namespace agile {
namespace logger {

LoggerBufferImp::LoggerBufferImp(const LoggerConfig* config)
{
    config_ = config;
    logger_str_ = std::make_shared<std::string>();
    logger_str_->resize(config_->logger_buffer_size);
}

bool LoggerBufferImp::Reserve(size_t data_size)
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

void LoggerBufferImp::Write(const std::string& val) 
{
    if (Reserve(val.length()))
    {
        memcpy(&logger_str_->at(write_index_), val.c_str(), val.length());
        write_index_ += val.length();
    }
}

void LoggerBufferImp::Write(const char* val) 
{
    size_t len = strlen(val);
    if (Reserve(len))
    {
        memcpy(&logger_str_->at(write_index_), val, len);
        write_index_ += len;
    }
}

} // namespace logger
} // namespace agile
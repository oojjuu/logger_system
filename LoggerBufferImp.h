#pragma once

#include "LoggerBuffer.h"

namespace agile {
namespace logger {

/**
*@brief 日志buffer接口实现类
*/
class LoggerBufferImp : public LoggerBuffer
{
public:
    LoggerBufferImp(const LoggerConfig* config);

    ~LoggerBufferImp(){ }

    /**
    *@brief 获取写数据游标
    *@return 游标值
    */
    virtual size_t GetWriteIndex() const { return write_index_; }
    
    /**
    *@brief 设置写数据游标
    *@param val 游标值
    */
    virtual void SetWriteIndex(size_t val) { write_index_ = val; }

    /**
    *@brief 获取buffer数据
    *@return string智能指针引用
    */
    virtual std::shared_ptr<std::string>& Data() { return logger_str_; }

    /**
    *@brief 获取日志配置信息
    *@return const LoggerConfig*
    */
    virtual const LoggerConfig* GetConfig() const { return config_; }

    /**
    *@brief 检查buffer容量并做扩容操作
    *@param data_size 数据需要的空间
    *@return bool
    */
    virtual bool Reserve(size_t data_size);

private:
    /**
    *@brief 以下是不同类型数据写入
    *@param val 不同类型数值
    */
    virtual void Write(const std::string& val);

    virtual void Write(const char* val);

private:
    // 日志配置信息
    const LoggerConfig* config_ = nullptr;
    // buffer写入游标
    size_t write_index_ = 0;
    // 日志数据, 字符串类型
    std::shared_ptr<std::string> logger_str_ = nullptr;
};

} // namespace logger
} // namespace agile

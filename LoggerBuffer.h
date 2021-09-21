#pragma once

#include <memory>
#include <string>

#include "LoggerConfig.h"

namespace agile {
namespace logger {

/**
*@brief 日志buffer类
*/
class LoggerBuffer
{
public:
    LoggerBuffer();

    virtual ~LoggerBuffer();

    /**
    *@brief 获取写数据游标
    *@return 游标值
    */
    virtual size_t GetWriteIndex() const;
    
    /**
    *@brief 设置写数据游标
    *@param val 游标值
    */
    virtual void SetWriteIndex(size_t val);

    /**
    *@brief 获取buffer数据
    *@return string智能指针引用
    */
    virtual std::shared_ptr<std::string>& Data();

    /**
    *@brief 获取日志配置信息
    *@return const LoggerConfig*
    */
    virtual const LoggerConfig* GetConfig() const;

    /**
    *@brief 检查buffer容量并做扩容操作
    *@param data_size 数据需要的空间
    *@return bool
    */
    virtual bool Reserve(size_t data_size);

    /**
    *@brief 以下是不同类型数据操作符重载
    *@param val 不同类型数值
    *@return LoggerBuffer实例
    */
    LoggerBuffer& operator << (bool val);

    LoggerBuffer& operator << (int8_t val);
    LoggerBuffer& operator << (uint8_t val);

    LoggerBuffer& operator << (int16_t val);
    LoggerBuffer& operator << (uint16_t val);

    LoggerBuffer& operator << (int32_t val);
    LoggerBuffer& operator << (uint32_t val);

    LoggerBuffer& operator << (int64_t val);
    LoggerBuffer& operator << (uint64_t val);

    LoggerBuffer& operator << (float val);
    LoggerBuffer& operator << (double val);

    LoggerBuffer& operator << (long long val);
    LoggerBuffer& operator << (unsigned long long val);

    LoggerBuffer& operator << (const std::string& val);
    LoggerBuffer& operator << (const char* val); 

protected:
    /**
    *@brief 以下是不同类型数据写入
    *@param val 不同类型数值
    */
    template<typename T>
    void Write(T val, const std::string& flag);
    
    virtual void Write(const std::string& val);

    virtual void Write(const char* val);
};

} // namespace logger
} // namespace agile

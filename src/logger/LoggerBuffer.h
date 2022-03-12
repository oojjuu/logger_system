#pragma once

#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <bitset>
#include <unordered_set>
#include <map>
#include <unordered_map>

#include "LoggerConfig.h"

namespace agile {
namespace logger {

/**
*@brief 日志buffer类
*/
class LoggerBuffer {
public:
    LoggerBuffer() = default;
    LoggerBuffer(const LoggerConfig* config);

    virtual ~LoggerBuffer() = default;

    /**
    *@brief 获取写数据游标
    *@return 游标值
    */
    virtual size_t write_index() const { return write_index_; }
    
    /**
    *@brief 设置写数据游标
    *@param val 游标值
    */
    virtual void set_write_index(size_t val) { write_index_ = val; }

    /**
    *@brief 移动写数据游标
    *@param val 移动游标值
    */
    virtual void incr_write_index_by(size_t val) { write_index_ += val; }

    /**
    *@brief 获取buffer数据
    *@return string智能指针引用
    */
    virtual std::shared_ptr<std::string>& data() { return logger_str_; }

    /**
    *@brief 获取日志配置信息
    *@return const LoggerConfig* 日志配置
    */
    virtual const LoggerConfig* config() const { return config_; }
    
    /**
    *@brief 设置使能
    */
    void set_enable(bool val) { enable_ = val; }

    bool enable() const { return enable_; }

    /**
    *@brief 获取当前游标地址
    *@return const char指针
    */
    virtual char* cur_data() { return &logger_str_->at(write_index_); }

    /**
    *@brief 检查buffer容量并做扩容操作
    *@param data_size 数据需要的空间
    *@return bool
    */
    virtual bool Reserve(size_t data_size);

    /**
    *@brief 下面是基础类型数据写入重载
    *@param val 数据值
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

    /**
    *@brief char*类型数据写入重载
    *@param val char指针
    *@return LoggerBuffer实例
    */
    LoggerBuffer& operator << (const char* val);

    /**
    *@brief char*类型数据写入重载
    *@param val string
    *@return LoggerBuffer实例
    */
    LoggerBuffer& operator << (const std::string& val);

    /**
    *@brief 指针类型数据写入重载
    *@param val 指针数据
    *@return LoggerBuffer实例
    */
    template<typename T, 
    typename std::enable_if<std::is_pointer<T>::value && !std::is_same<T, const char*>::value, T>::type* = nullptr>
    LoggerBuffer& operator << (const T& val) {
        *this << *val;
        return *this;
    }

    /**
    *@brief 智能指针类型数据写入重载
    *@param val 智能指针
    *@return LoggerBuffer实例
    */
    template<typename T>
    LoggerBuffer& operator << (const std::shared_ptr<T>& val) {
        *this << *val;
        return *this;
    }

    /**
    *@brief pair类型数据重载函数
    *@param val pair类型值
    *@return LoggerBuffer实例
    */
    template<typename T1, typename T2>
    LoggerBuffer& operator << (const std::pair<T1, T2>& val) {
        *this << kBracketLeftString << val.first << kStlMapEmptySplitString << val.second << kBracketRightString;
        return *this;
    }

    /**
    *@brief 容器类型数据重载函数
    *@param val 容器类型值
    *@return LoggerBuffer实例
    */
    template<typename T, 
    typename std::enable_if<std::is_class<T>::value && !std::is_same<T, std::string>::value, T>::type* = nullptr>
    LoggerBuffer& operator << (const T& val) {
        DoWrite<T>(val, 0);
        return *this;
    }

private:
    template<typename T, void (T::*)(LoggerBuffer&) const>
    struct WriterFunc;

    template<typename T>
    void DoWrite(const T& val, WriterFunc<T, &T::OnAgileLogger>*) {
        if (!enable_) {
            return;
        }
        val.OnAgileLogger(*this);
    }

    template<typename T>
    void DoWrite(const T& val, ...) {
        if (!enable_) {
            return;
        }

        if (val.empty()) {
            *this << kStlEmptyString;
            return;
        }
        
        auto it = val.begin();
        *this << kBracketLeftString << *it;
        while (++it != val.end()) {
            *this << kStlEmptySplitString << *it;
        }
        *this << kBracketRightString;
    }

    /**
    *@brief 写入基础类型格式化数据
    *@param val string
    */
    template<typename T>
    void WriteWithFmt(T val, const std::string& fmt) {
        if (!enable_) {
            return;
        }
        // 为数值类型预留的space
        static constexpr size_t kNumberTypeMaxSizeLimited = 64;
        if (Reserve(kNumberTypeMaxSizeLimited)) {
            write_index_ += sprintf(&logger_str_->at(write_index_), fmt.c_str(), val);
        }
    }

private:
    // bool 
    bool enable_ = true;
    // 日志配置信息
    const LoggerConfig* config_ = nullptr;
    // buffer写入游标
    size_t write_index_ = 0;
    // 日志数据, 字符串类型
    std::shared_ptr<std::string> logger_str_ = nullptr;

    // 字符常量
    static const std::string kStlEmptyString;
    static const std::string kStlEmptySplitString;
    static const std::string kStlMapEmptySplitString;

    static const std::string kBracketLeftString;
    static const std::string kBracketRightString;
};

} // namespace logger
} // namespace agile
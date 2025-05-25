#ifndef AGILE_LOGGER_LOGGER_BUFFER_H
#define AGILE_LOGGER_LOGGER_BUFFER_H

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

namespace agile {
namespace logger {
class LoggerBuffer {
public:
    LoggerBuffer() = delete;
    LoggerBuffer(size_t bufferSize, size_t bufferSizeLimit);
    virtual ~LoggerBuffer() = default;

    size_t GetWriteIndex() const { return wIndex_; }
    void SetWriteIndex(size_t val) { wIndex_ = val; }

    std::shared_ptr<std::string>& data() { return loggerStr_; }
    bool Reserve(size_t size);

    virtual LoggerBuffer& operator << (bool val);
    virtual LoggerBuffer& operator << (int8_t val);
    virtual LoggerBuffer& operator << (uint8_t val);
    virtual LoggerBuffer& operator << (int16_t val);
    virtual LoggerBuffer& operator << (uint16_t val);
    virtual LoggerBuffer& operator << (int32_t val);
    virtual LoggerBuffer& operator << (uint32_t val);
    virtual LoggerBuffer& operator << (int64_t val);
    virtual LoggerBuffer& operator << (uint64_t val);
    virtual LoggerBuffer& operator << (float val);
    virtual LoggerBuffer& operator << (double val);
    virtual LoggerBuffer& operator << (const char* val);
    virtual LoggerBuffer& operator << (const std::string& val);

    template<typename T,
    typename std::enable_if<std::is_pointer<T>::value && !std::is_same<T, const char*>::value, T>::type* = nullptr>
    LoggerBuffer& operator << (const T& val) {
        *this << *val;
        return *this;
    }

    template<typename T>
    LoggerBuffer& operator << (const std::shared_ptr<T>& val) {
        *this << *val;
        return *this;
    }

    template<typename T1, typename T2>
    LoggerBuffer& operator << (const std::pair<T1, T2>& val) {
        *this << "{" << val.first << ":" << val.second << "}";
        return *this;
    }

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
        val.OnAgileLogger(*this);
    }

    template<typename T>
    void DoWrite(const T& val, ...)
    {
        if (val.empty()) {
            *this << "{}";
            return;
        }
        auto it = val.begin();
        *this << "{" << *it;
        while (++it != val.end()) {
            *this << "," << *it;
        }
        *this << "}";
    }

protected:
    size_t wIndex_ = 0;
    size_t bufferSizeLimit_ = 0;
    std::shared_ptr<std::string> loggerStr_ = nullptr;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_BUFFER_H

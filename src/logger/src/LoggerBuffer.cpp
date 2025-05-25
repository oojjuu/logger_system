#include "LoggerBuffer.h"

#include <iostream>
#include <cstring>
#include "LoggerUtil.h"

namespace agile {
namespace logger {
static constexpr size_t kNumberTypeLimited = 30;
LoggerBuffer::LoggerBuffer(size_t bufferSize, size_t bufferSizeLimit)
{
    loggerStr_ = std::make_shared<std::string>();
    loggerStr_->resize(bufferSize);
    bufferSizeLimit_ = bufferSizeLimit;
}

bool LoggerBuffer::Reserve(size_t size)
{
    size_t curSize = size + wIndex_;
    if (curSize < loggerStr_->size()) {
        return true;
    }
    if (curSize > bufferSizeLimit_) {
        std::cerr << "logger length " << curSize << " out of limited size " <<
            bufferSizeLimit_ << std::endl;
        return false;
    }

    const size_t capacitySize = loggerStr_->capacity();
    if (curSize < capacitySize) {
        loggerStr_->resize(capacitySize);
        return true;
    }
    
    curSize += curSize;
    if (curSize > bufferSizeLimit_) {
        loggerStr_->resize(bufferSizeLimit_);
    } else {
        loggerStr_->resize(curSize);
    }
    return true;
}

LoggerBuffer& LoggerBuffer::operator << (bool val)
{
    static constexpr size_t kBoolTypeSizeLimited = 6;
    if (Reserve(kBoolTypeSizeLimited)) {
        if (val) {
            (*loggerStr_)[wIndex_++] = 't';
            (*loggerStr_)[wIndex_++] = 'r';
            (*loggerStr_)[wIndex_++] = 'u';
            (*loggerStr_)[wIndex_++] = 'e';
        } else {
            (*loggerStr_)[wIndex_++] = 'f';
            (*loggerStr_)[wIndex_++] = 'a';
            (*loggerStr_)[wIndex_++] = 'l';
            (*loggerStr_)[wIndex_++] = 's';
            (*loggerStr_)[wIndex_++] = 'e';
        }
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (int8_t val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::NumToString(*loggerStr_, wIndex_, static_cast<int64_t>(val));
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (uint8_t val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::NumToString(*loggerStr_, wIndex_, static_cast<uint64_t>(val));
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (int16_t val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::NumToString(*loggerStr_, wIndex_, static_cast<int64_t>(val));
    }
    return *this;
}    
    
LoggerBuffer& LoggerBuffer::operator << (uint16_t val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::NumToString(*loggerStr_, wIndex_, static_cast<uint64_t>(val));
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (int32_t val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::NumToString(*loggerStr_, wIndex_, static_cast<int64_t>(val));
    }
    return *this;
}
    
LoggerBuffer& LoggerBuffer::operator << (uint32_t val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::NumToString(*loggerStr_, wIndex_, static_cast<uint64_t>(val));
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (int64_t val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::NumToString(*loggerStr_, wIndex_, val);
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (uint64_t val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::NumToString(*loggerStr_, wIndex_, val);
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (float val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::FloatToString(*loggerStr_, wIndex_, static_cast<double>(val));
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (double val)
{
    if (Reserve(kNumberTypeLimited)) {
        wIndex_ += LoggerUtil::FloatToString(*loggerStr_, wIndex_, val);
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (const char* val)
{
    const size_t len = strlen(val);
    if (Reserve(len)) {
        memcpy(&loggerStr_->at(wIndex_), val, len);
        wIndex_ += len;
    }
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (const std::string& val)
{
    const size_t len = val.length();
    if (Reserve(len)) {
        memcpy(&loggerStr_->at(wIndex_), val.c_str(), len);
        wIndex_ += len;
    }
    return *this;
}
} // namespace logger
} // namespace agile

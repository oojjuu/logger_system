#include "LoggerBuffer.h"

namespace agile {
namespace logger {

// 为数值类型预留的space
static constexpr size_t kNumberTypeMaxSizeLimited = 64;

LoggerBuffer::LoggerBuffer(){ }

LoggerBuffer::~LoggerBuffer(){ }

void LoggerBuffer::SetWriteIndex(size_t val){ }

size_t LoggerBuffer::GetWriteIndex() const
{
    return 0;
}

bool LoggerBuffer::Reserve(size_t data_size)
{
    return true;
}

const LoggerConfig* LoggerBuffer::GetConfig() const
{
    return nullptr;
}

static std::shared_ptr<std::string> kBufferDefaultString = std::make_shared<std::string>();

std::shared_ptr<std::string>& LoggerBuffer::Data()
{
    return kBufferDefaultString;
}

#define BLOperatorImp(data_type, format) \
        LoggerBuffer& LoggerBuffer::operator << (data_type data_val) \
        { \
            Write(data_val, format); \
            return *this; \
        }


LoggerBuffer& LoggerBuffer::operator << (bool val)
{
    if (val)
    {
        Write("true");
    }
    else
    {
        Write("false");
    }
    return *this;
}

BLOperatorImp(int8_t, "%d")
BLOperatorImp(uint8_t, "%u")

BLOperatorImp(int16_t, "%d")
BLOperatorImp(uint16_t, "%u")

BLOperatorImp(int32_t, "%d")
BLOperatorImp(uint32_t, "%u")

BLOperatorImp(int64_t, "%ld")
BLOperatorImp(uint64_t, "%lu")

BLOperatorImp(float, "%f")
BLOperatorImp(double, "%f")

BLOperatorImp(long long, "%lld")
BLOperatorImp(unsigned long long, "%llu")

LoggerBuffer& LoggerBuffer::operator << (const std::string& val)
{
    Write(val);
    return *this;
}

LoggerBuffer& LoggerBuffer::operator << (const char* val)
{
    Write(val);
    return *this;
}

template<typename T>
void LoggerBuffer::Write(T val, const std::string& flag)
{
    if (Reserve(kNumberTypeMaxSizeLimited))
    {
        size_t write_index = GetWriteIndex();
        std::shared_ptr<std::string>& logger_str = Data();
        write_index += sprintf(&logger_str->at(write_index), flag.c_str(), val);
        SetWriteIndex(write_index);
    }
}

void LoggerBuffer::Write(const std::string& val){ }
    
void LoggerBuffer::Write(const char* val){ }

} // namespace logger
} // namespace agile
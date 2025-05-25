#ifndef AGILE_LOGGER_LOGGER_FILE_HANDLER_H
#define AGILE_LOGGER_LOGGER_FILE_HANDLER_H

#include <sys/types.h>
#include <string>
#include <atomic>

#include "LoggerData.h"

namespace agile {
namespace logger {
class FileHandler {
public:
    FileHandler() = default;
    FileHandler(const std::string& filePath);
    virtual ~FileHandler();

    virtual bool Write(const char *data, uint32_t len) = 0;

    virtual bool Flush() { return true; }

    virtual bool Sync() { return true; }

protected:
    FILE* fileHandler_ = nullptr;
};

class EmptyFileHandler : public FileHandler {
public:
    EmptyFileHandler() = default;
    EmptyFileHandler(const std::string& filePath) = delete;
    virtual ~EmptyFileHandler() = default;
    bool Write(const char *data, uint32_t len)
    {
        static_cast<void>(data);
        static_cast<void>(len);
        return true;
    }
};

class SyncFileHandler : public FileHandler {
public:
    SyncFileHandler() = delete;
    SyncFileHandler(const std::string& filePath) : FileHandler(filePath) { }
    virtual ~SyncFileHandler()  = default;

    bool Write(const char *data, uint32_t len);
};

class AsyncFileHandler : public FileHandler {
public:
    AsyncFileHandler() = delete;
    AsyncFileHandler(const std::string& filePath) : FileHandler(filePath) { }
    virtual ~AsyncFileHandler()  = default;

    bool Write(const char *data, uint32_t len);
    bool Flush();
    bool Sync();
};

class AsyncFileHandlerSync : public AsyncFileHandler
{
public:
    AsyncFileHandlerSync() = delete;
    AsyncFileHandlerSync(const std::string& filePath, uint32_t maxCounter) : AsyncFileHandler(filePath),
        maxCounter_{maxCounter} { }
    ~AsyncFileHandlerSync()  = default;

    bool Write(const char *data, uint32_t len);
    bool Sync();

private:
    uint32_t maxCounter_{0};
    std::atomic<uint32_t> counter_{0};
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_FILE_HANDLER_H

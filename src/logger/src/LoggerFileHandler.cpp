#include "LoggerFileHandler.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <exception>
#include <iostream>

namespace agile {
namespace logger {
FileHandler::FileHandler(const std::string& filePath)
{
    fileHandler_ = fopen(filePath.c_str(), "a+");
    if (fileHandler_ != nullptr) {
        return;
    }
    std::cerr << "fail to open file=" << filePath << ", err=" << strerror(errno) << std::endl;
    throw std::exception();
}

FileHandler::~FileHandler()
{
    if (fileHandler_ != nullptr) {
        fclose(fileHandler_);
        fileHandler_ = nullptr;
    }
}

bool SyncFileHandler::Write(const char *data, uint32_t len)
{
    int res = fwrite(data, len, 1, fileHandler_);
    if (res != 1) {
        std::cerr << "[sync] fail to write res=" << res  << ", err=" << strerror(errno) << std::endl;
        return false;
    }
    if (0 != fflush(fileHandler_)) {
        std::cerr << "[sync] fail to flush err=" << strerror(errno) << std::endl;
        return false;
    }
    if (0 != fsync(fileno(fileHandler_))) {
        std::cerr << "[sync] fail to fsync err=" << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool AsyncFileHandler::Write(const char *data, uint32_t len)
{
    int res = fwrite(data, len, 1, fileHandler_);
    if (res != 1) {
        std::cerr << "[async] fail to write res=" << res  << ", err=" << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool AsyncFileHandler::Flush()
{
    if (0 != fflush(fileHandler_)) {
        std::cerr << "[async] fail to flush err=" << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool AsyncFileHandler::Sync()
{
    if (0 != fsync(fileno(fileHandler_))) {
        std::cerr << "[async] fail to fsync err=" << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool AsyncFileHandlerSync::Write(const char *data, uint32_t len)
{
    int res = fwrite(data, len, 1, fileHandler_);
    if (res != 1) {
        std::cerr << "[async0] fail to write res=" << res  << ", err=" << strerror(errno) << std::endl;
        return false;
    }
    ++counter_;
    if (counter_ >= maxCounter_) {
        static_cast<void>(Flush());
        static_cast<void>(Sync());
    }
    return true;
}

bool AsyncFileHandlerSync::Sync()
{
    if (0 != fsync(fileno(fileHandler_))) {
        std::cerr << "[async0] fail to fsync err=" << strerror(errno) << std::endl;
        return false;
    }
    counter_ = 0;
    return true;
}
} // namespace logger
} // namespace agile

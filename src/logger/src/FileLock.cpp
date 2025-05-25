#include "FileLock.h"
#include <cerrno>
#include <cstring>

namespace agile {
namespace logger {
FileLock::FileLock(const std::string &filePath, int32_t mode, int32_t chmod, int32_t lockType) :
    fd_{-1}, filePath_{filePath}, mode_{mode}, chmod_{chmod}, lockType_{lockType}
{
}

FileLock::~FileLock()
{
    static_cast<void>(UnLock());
}

bool FileLock::Lock()
{
    fd_ = open(filePath_.c_str(), mode_, chmod_);
    if (fd_ <= 0) {
        err_.SaveErrno();
        return false;
    }
    if (flock(fd_, lockType_) != 0) {
        close(fd_);
        fd_ = -1;
        err_.SaveErrno();
        return false;
    }
    return true;
}

bool FileLock::UnLock()
{
    int32_t res = 0;
    if (fd_ != -1) {
        res = flock(fd_, LOCK_UN);
        if (res != 0) {
            err_.SaveErrno();
        }
        close(fd_);
        fd_ = -1;
    }
    return res == 0;
}

std::string FileLock::GetError() const
{
    return err_.ToString();
}
} // logger
} // agile

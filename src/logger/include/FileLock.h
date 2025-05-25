#ifndef AGILE_FILE_LOCK_H
#define AGILE_FILE_LOCK_H

#include <string>
#include <unistd.h>
#include <sys/file.h>

#include "ErrorMessage.h"

namespace agile {
namespace logger {
class FileLock final {
public:
    FileLock(const std::string &filePath, int32_t mode = O_WRONLY | O_CREAT, int32_t chmod = 0666,
        int32_t lockType = LOCK_EX);
	FileLock() = delete;
	~FileLock();
	
	bool Lock();
    bool UnLock();
    std::string GetError() const;

private:
    int32_t fd_;
    std::string filePath_;
    int32_t mode_;
    int32_t chmod_;
    int32_t lockType_;
    ErrorMessage err_;
};
} // namespace logger
} // namespace agile
#endif // AGILE_FILE_LOCK_H

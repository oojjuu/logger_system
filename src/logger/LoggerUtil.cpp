#include "LoggerUtil.h"

#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#ifdef __linux__
#include <sys/statfs.h>
#else
#include <sys/param.h>
#include <sys/mount.h>
#endif // __linux__

namespace agile {
namespace logger {

long long LoggerUtil::GetDiskFreeCapacity(const std::string& path) {
    struct statfs buffer;
    int res = statfs(path.c_str(), &buffer);
    return res < 0 ? res : (long long)buffer.f_bsize * (long long)buffer.f_bfree;
}

int64_t LoggerUtil::GetFileSize(const std::string& file_name) {
    struct stat statbuf;
    return 0 == stat(file_name.c_str(), &statbuf) ? statbuf.st_size : -1;
}

std::string LoggerUtil::RenameLogingToLog(const std::string& loging_file) {
    static const std::string kLogingString = "loging";
    if (loging_file.length() <= kLogingString.length()) {
        return "";
    }
    size_t index = loging_file.length() - kLogingString.length();
    for (size_t i = 0; i < kLogingString.length(); i++) {
        if (loging_file[i + index] != kLogingString[i]) {
            return "";
        }
    }
    if (0 != access(loging_file.c_str(), 0)) {
        return "";
    }
    std::string new_name = loging_file.substr(0, loging_file.length() - 3);
    rename(loging_file.c_str(), new_name.c_str());
    return new_name;
}

std::string LoggerUtil::GetLoggerFileNameByLoggerId(const LoggerConfig* config, const std::string& name_tag, 
                                        uint32_t logger_id, uint32_t& new_logger_id)
{
    std::string file_path = config->logger_dir_path;
    if (file_path.back() != '/') {
        file_path += "/";
    }
    file_path += config->logger_name;
    if (!name_tag.empty()) {
        file_path += ("_" + name_tag);
    }
    file_path += "_";

    uint32_t temp_id = logger_id;
    std::string file_name_loging = file_path + std::to_string(logger_id) + ".loging";
    std::string file_name_log = file_path + std::to_string(logger_id) + ".log";
    int loop_num = (int)config->logger_file_number + 1;
    int ret1 = access(file_name_loging.c_str(), 0);
    int ret2 = access(file_name_log.c_str(), 0);
    while ( (0 == ret1 || 0 == ret2 ) && --loop_num > 0) {
        if (0 == ret1) {
            int64_t file_size = GetFileSize(file_name_loging);
            if (file_size > 0 && file_size < (int64_t)config->logger_file_size) {
                break;
            }
        }
        file_name_loging = file_path + std::to_string(++logger_id) + ".loging";
        file_name_log = file_path + std::to_string(logger_id) + ".log";
    }
    if (loop_num < 1) {
        logger_id = temp_id;
        file_name_loging = file_path + std::to_string(logger_id) + ".loging";
        remove(file_name_loging.c_str());
    }
    new_logger_id = logger_id;
    return file_name_loging;
}

uint32_t LoggerUtil::GetLoggerOrderIds(const std::vector<std::string>& files_name, std::vector<int64_t>& logger_ids) {
    if (files_name.empty()) {
        return 0;
    }
    size_t size = files_name.size();
    logger_ids.resize(size);

    uint32_t max_id = 0;
    for (size_t i = 0; i < size; i++) {
        const std::string& str = files_name[i];
        size_t pos1 = str.rfind('.');
        size_t pos2 = str.rfind('_');
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            logger_ids[i] = -1;
            continue;
        }
        std::string num_str = str.substr(pos2 + 1, pos1 - pos2 - 1);
        uint32_t id_val = (uint32_t)atol(num_str.c_str());
        logger_ids[i] = (int64_t)id_val;
        if (id_val >= max_id) {
            max_id = id_val + 1;
        }
    }
    return max_id;
}

bool LoggerUtil::GetLoggerFiles(const std::string& path, const std::string& fliter, std::vector<std::string>& res) {
    DIR* direct_ptr = opendir(path.c_str());
    if (!direct_ptr) {
        std::cerr << "fail to opendir dir_path:" << path << std::endl;
        return false;
    }
    std::string pre_path = path;
    if (pre_path.back() != '/') {
        pre_path += "/";
    }
    struct dirent* entry = nullptr;
    while ((entry = readdir(direct_ptr))) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        std::string name = entry->d_name;
        std::string file_path = pre_path + name;
        struct stat buffer;
        if(!(stat(file_path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode))) {
            continue;
        }
        if (name.find(fliter) != std::string::npos) {
            res.emplace_back(std::move(file_path));
        }
    }
    return true;
}

bool LoggerUtil::CheckAndCreateLogDir(const std::string& dir_path) {
    if (dir_path.empty()) {
        std::cerr << "dir_path is empty" << std::endl;
        return false;
    }

    DIR* dir = opendir(dir_path.c_str());
    if (dir) {
        closedir(dir);
        return true;
    }
    
    std::string path = dir_path;
    if (path.back() != '/') {
        path += '/';
    }
    size_t pre_index = 0;
    size_t pos = 0;
    while ((pos = path.find_first_of('/', pre_index)) != std::string::npos) {
        std::string dir_str = path.substr(0, pos++);
        pre_index = pos;
        if (dir_str.empty()) {
            continue;
        }
        if (0 == mkdir(dir_str.c_str(), 0777)) {
            continue;
        }
        if (errno != EEXIST) {
            std::cerr << "fail to mkdir dir_path:" << dir_str << std::endl;
            return false;
        }
    }
    return true;
}

} // namespace logger
} // namespace agile
#include "LoggerUtil.h"

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#ifdef __linux__
#include <sys/statfs.h>
#else
#include <sys/param.h>
#include <sys/mount.h>
#endif // __linux__
#include <filesystem>

namespace agile {
namespace logger {
long long LoggerUtil::GetDiskFreeCapacity(const std::string& path)
{
    struct statfs buffer;
    int res = statfs(path.c_str(), &buffer);
    return res < 0 ? res : static_cast<long long>(buffer.f_bsize) * static_cast<long long>(buffer.f_bfree);
}

int64_t LoggerUtil::GetFileSize(const std::string& path)
{
    struct stat statbuf;
    return 0 == stat(path.c_str(), &statbuf) ? statbuf.st_size : 0;
}

std::set<int32_t> LoggerUtil::GetFileUsers(const std::string& filePath)
{
    std::set<int32_t> pids;
    const std::string cmd = "lsof -t " + filePath + " 2>/dev/null";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        return pids;
    }
    constexpr uint32_t bufferSize = 1024;
    char buffer[bufferSize];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        std::stringstream ss(buffer);
        int32_t counter = 100;
        int32_t pid = 0;
        while (ss >> pid) {
            static_cast<void>(pids.insert(pid));
            if (--counter < 0) {
                break;
            }
        }
    }
    return pids;
}

bool LoggerUtil::HasOtherUser(const std::string& filePath)
{
    static pid_t selfPid = getpid();
    std::set<int32_t> pids = LoggerUtil::GetFileUsers(filePath);
    for (auto it : pids) {
        if (it != selfPid) {
            return true;
        }
    }
    return false;
}

bool LoggerUtil::RenameFileWithoutOtherUsers(const std::string &oldName, const std::string &newName)
{
    if (HasOtherUser(oldName)) {
        return false;
    }
    if (std::filesystem::exists(newName)) {
        return false;
    }
    if (rename(oldName.c_str(), newName.c_str()) != 0) {
        return false;
    }
    return true;
}

std::string LoggerUtil::RenameLogingToLog(const std::string& loggingFile, bool force)
{
    constexpr uint32_t ingLen = 3;
    if (loggingFile.length() < ingLen) {
        return "";
    }
    std::string newName = loggingFile.substr(0, loggingFile.length() - ingLen);
    if (force) {
        static_cast<void>(rename(loggingFile.c_str(), newName.c_str()));
        return newName;
    }
    if (!RenameFileWithoutOtherUsers(loggingFile, newName)) {
        return "";
    }
    return newName;
}

bool LoggerUtil::IsLogFile(const std::string& filePath)
{
    const std::string log = ".log";
    if (filePath.length() < log.length()) {
        return false;
    }
    return filePath.substr(filePath.length() - log.length()) == log;
}

bool LoggerUtil::IsLogingFile(const std::string& filePath)
{
    const std::string loging = ".loging";
    if (filePath.length() < loging.length()) {
        return false;
    }
    return filePath.substr(filePath.length() - loging.length()) == loging;
}

std::string LoggerUtil::CreateLoggerFileName(const LoggerConfig* config, const std::string& tag,
    uint32_t loggerId, bool loging)
{
    std::string filePath = config->logger_dir_path;
    if (filePath.back() != '/') {
        filePath += "/";
    }
    filePath += config->logger_name;
    if (!tag.empty()) {
        filePath += ("_" + tag);
    }
    filePath += ("_" + std::to_string(loggerId));
    if (loging) {
        filePath += ".loging";
    } else {
        filePath += ".log";
    }
    return filePath;
}

static void UpdateListHelper(uint32_t initId, uint32_t loggerId, std::list<uint32_t> &loggerIds)
{
    if (initId != loggerId) {
        loggerIds.push_back(loggerId);
    }
}

std::string LoggerUtil::GetLoggerFileName(const LoggerConfig* config, const std::string& tag,
    uint32_t& loggerId, std::list<uint32_t> &loggerIds)
{
    std::string filePath = config->logger_dir_path;
    if (filePath.back() != '/') {
        filePath += "/";
    }
    filePath += config->logger_name;
    if (!tag.empty()) {
        filePath += ("_" + tag);
    }
    filePath += "_";

    const uint32_t tempId = loggerId;
    int32_t counter = static_cast<int32_t>(config->logger_file_number + config->logger_file_number);
    std::string logingFile;
    std::string logFile;
    while (--counter >= 0 ) {
        logFile = filePath + std::to_string(loggerId) + ".log";
        if (std::filesystem::exists(logFile)) {
            UpdateListHelper(tempId, loggerId, loggerIds);
            loggerId++;
            continue;
        }
        logingFile = filePath + std::to_string(loggerId) + ".loging";
        if (!std::filesystem::exists(logingFile)) {
            break;
        }
        if (!HasOtherUser(logingFile)) {
            RenameLogingToLog(logingFile);
            UpdateListHelper(tempId, loggerId, loggerIds);
            loggerId++;
            continue;
        }
        const int64_t fileSize = static_cast<int64_t>(GetFileSize(logingFile));
        if ((fileSize >= 0) && (fileSize < static_cast<int64_t>(config->logger_file_size))) {
            break;
        }
        UpdateListHelper(tempId, loggerId, loggerIds);
        loggerId++;
    }
    if (counter < 0) {
        loggerId = tempId;
        logingFile = filePath + std::to_string(loggerId) + ".loging";
        const std::string logingFileBackup = logingFile + "_backup";
        static_cast<void>(rename(logingFile.c_str(), logingFileBackup.c_str()));
    }
    return logingFile;
}

bool LoggerUtil::IsAllNumberChar(const std::string &str)
{
    if (str.empty()) {
        return false;
    }
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}

std::map<uint32_t, std::string> LoggerUtil::GetLoggerOrderIds(const std::vector<std::string>& fileNames)
{
    std::map<uint32_t, std::string> loggerIds;
    if (fileNames.empty()) {
        return loggerIds;
    }
    const size_t size = fileNames.size();
    for (size_t i = 0; i < size; i++) {
        const std::string& str = fileNames[i];
        size_t pos1 = str.rfind('.');
        size_t pos2 = str.rfind('_');
        if ((pos1 == std::string::npos) || (pos2 == std::string::npos)) {
            continue;
        }
        const std::string num = str.substr(pos2 + 1, pos1 - pos2 - 1);
        if (!IsAllNumberChar(num)) {
            continue;
        }
        const uint32_t idVal = static_cast<uint32_t>(atol(num.c_str()));
        loggerIds[idVal] = fileNames[i];
    }
    return loggerIds;
}

std::vector<std::string> LoggerUtil::GetLoggerFiles(const std::string& path, const std::string& fliter)
{
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        size_t len = fliter.length();
        const std::string fileName = entry.path().filename();
        if (fileName.length() < len + 5) {
            continue;
        }
        if (!IsLogFile(fileName)) {
            if (!IsLogingFile(fileName)) {
                continue;
            }
        }
        if (fileName.substr(0, len) != fliter) {
            continue;
        }
        size_t pos = fileName.rfind('.');
        if (pos == std::string::npos || pos <= len) {
            continue;
        }
        const std::string num = fileName.substr(len, pos - len);
        if (!IsAllNumberChar(num)) {
            continue;
        }
        files.push_back(entry.path().string());
    }
    return files;
}

bool LoggerUtil::CreateDirectory(const std::string& path)
{
    if (path.empty()) {
        std::cerr << "directory path is empty" << std::endl;
        return false;
    }
    bool result = false;
    if (std::filesystem::exists(path)) {
        result = true;
    } else {
        result = std::filesystem::create_directories(path);
    }
    if (result) {
        std::filesystem::permissions(path, std::filesystem::perms::owner_all | std::filesystem::perms::group_read |
            std::filesystem::perms::others_read, std::filesystem::perm_options::replace);
    }
    return result;
}

uint32_t LoggerUtil::NumToString(std::string &buffer, uint32_t index, int64_t value)
{
    if (value < 0) {
        buffer[index] = '-';
        value = -value;
        return NumToString(buffer, index + 1, static_cast<uint64_t>(value)) + 1;
    } else {
        return NumToString(buffer, index, static_cast<uint64_t>(value));
    }
}

uint32_t LoggerUtil::NumToString(std::string &buffer, uint32_t index, uint64_t value)
{
    static thread_local std::string bufferStr = "123456789101234567890123456789";
    static constexpr uint32_t bufferSize = 30;
    static constexpr uint32_t bufferMaxIndex = 29;
    uint32_t num = 0;
    uint32_t bufferIndex = bufferMaxIndex;
    while (value != 0) {
        bufferStr[bufferIndex--] = '0' + (value % 10);
        value /= 10;
        num++;
    }
    memcpy(&buffer.at(index), &bufferStr[bufferSize - num], num);
    return num;
}

uint32_t LoggerUtil::FloatToStringHelper(std::string &buffer, uint32_t index, double fValue)
{
    static thread_local std::string bufferStr = "12345678910123456789012345678912345678910123456789";
    static constexpr uint32_t bufferSize = 50;
    static constexpr uint32_t bufferMaxIndex = 49;
    uint32_t num = 0;
    uint32_t bufferIndex = bufferMaxIndex;
    uint64_t intVal = static_cast<uint64_t>(fValue);
    double fPart = fValue -  static_cast<double>(intVal);
    uint64_t value = static_cast<uint64_t>(fPart * 1000.0f);
    bool flag = true;
    while (value != 0) {
        if (flag) {
            if (value % 10 == 0) {
                value /= 10;
                continue;
            }
        }
        flag = false;
        bufferStr[bufferIndex--] = '0' + (value % 10);
        value /= 10;
        ++num;
    }
    bufferStr[bufferIndex--] = '.';
    ++num;

    value = intVal;
    while (value != 0) {
        bufferStr[bufferIndex--] = '0' + (value % 10);
        value /= 10;
        ++num;
    }
    memcpy(&buffer.at(index), &bufferStr[bufferSize - num], num);
    return num;
}

uint32_t LoggerUtil::FloatToString(std::string &buffer, uint32_t index, double value)
{
    if (value < 0) {
        buffer[index] = '-';
        value = -value;
        return FloatToStringHelper(buffer, index + 1, value) + 1;
    } else {
        return FloatToStringHelper(buffer, index, value);
    }
}

struct tm LoggerUtil::GetLoggerTmTime()
{
    static constexpr uint32_t year = 1900;
	time_t timestamp = time(0);
	struct tm val = *localtime(&timestamp);
	val.tm_year += year;
	val.tm_mon++;
    return val;
}
} // namespace logger
} // namespace agile

#ifndef AGILE_LOGGER_LOGGER_UTIL_H
#define AGILE_LOGGER_LOGGER_UTIL_H

#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>

#include "LoggerConfig.h"

namespace agile {
namespace logger {
/**
*@brief Log Assistant class
*/
class LoggerUtil {
public:
    LoggerUtil() = default;
    virtual ~LoggerUtil() = default;

    /**
    *@brief Check and create the log directory
    *@param path Log directory
    */ 
    static bool CreateDirectory(const std::string& path); // to test and update auth

    /**
    *@brief Obtain the remaining space of the disk
    *@param path Log path
    @return long long Remaining disk space, unit: bytes
    */  
    static long long GetDiskFreeCapacity(const std::string& path);

    /**
    *@brief Obtain the size of the log file
    *@param path Log path
    *@return int64_t Log file size
    */
    static int64_t GetFileSize(const std::string& path);

    /**
    *@brief Obtain the list of log files
    *@param path Log path
    *@param fliter File name filtering characters
    *@return List of result files
    */
    static std::vector<std::string> GetLoggerFiles(const std::string& path, const std::string& fliter);

    /**
    *@brief Rename the file
    *@param loggingFile Log file name
    *@return string A new log file name
    */
    static std::string RenameLogingToLog(const std::string& loggingFile, bool force = false);

    /**
    *@brief Obtain the log file name through loggerId
    *@param config Log configuration data
    *@param tag Log name tag
    *@param loggerId Log ID ref
    *@return string A new log file name
    */
    static std::string GetLoggerFileName(const LoggerConfig* config, const std::string& tag, uint32_t& loggerId,
        std::list<uint32_t> &loggerIds);
    static std::string CreateLoggerFileName(const LoggerConfig* config, const std::string& tag,
        uint32_t loggerId, bool loging = false);
    /**
    *@brief Obtain the log sequence number ID
    *@param fileNames List of log file names
    *@return List of log ids
    */
    static std::map<uint32_t, std::string> GetLoggerOrderIds(const std::vector<std::string>& fileNames);
    static bool IsLogFile(const std::string& filePath);
    static bool IsLogingFile(const std::string& filePath);
    
    static std::set<int32_t> GetFileUsers(const std::string& filePath);
    static bool HasOtherUser(const std::string& filePath);
    static bool RenameFileWithoutOtherUsers(const std::string &oldName, const std::string &newName);

    static uint32_t NumToString(std::string &buffer, uint32_t index, int64_t value);
    static uint32_t NumToString(std::string &buffer, uint32_t index, uint64_t value);
    static uint32_t FloatToString(std::string &buffer, uint32_t index, double value);
    static struct tm GetLoggerTmTime();

private:
    static bool IsAllNumberChar(const std::string &str);
    static uint32_t FloatToStringHelper(std::string &buffer, uint32_t index, double fValue);
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_UTIL_H

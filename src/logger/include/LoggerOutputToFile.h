#ifndef AGILE_LOGGER_LOGGER_OUTPUT_TO_FILE_H
#define AGILE_LOGGER_LOGGER_OUTPUT_TO_FILE_H

#include <mutex>
#include <chrono>

#include "FileLock.h"
#include "LoggerOutputImpl.h"
#include "LoggerConfig.h"
#include "LoggerFileHandlerList.h"

namespace agile {
namespace logger {
/**
*@brief Log output interface, implementing the OnLoggerMessage interface to output log files
*/
class LoggerOutputToFile : public LoggerOutputImpl {
public:
    LoggerOutputToFile() = delete;
    LoggerOutputToFile(const std::string& tag, const LoggerConfig* config);
    virtual ~LoggerOutputToFile();

    virtual void PreviousCheck(const std::shared_ptr<LoggerData>& loggerData) override;
	virtual void Run(const std::chrono::steady_clock::time_point& curTime, bool finished) override;   
    virtual void OnLoggerData(const std::shared_ptr<LoggerData>& loggerData) override;

private:
    /**
    *@brief Check the size of the log file regularly
    */ 
    void CheckFiles(const std::chrono::steady_clock::time_point& curTime);
    /**
    *@brief Check the disk regularly
    */ 
    void CheckDisk();
    /**
    *@brief Log file creation type
    */
    enum CreateType {
        CreateForInvalid = 0, // The file handle may be invalid
        CreateForFileSize = 1, // Created due to file size
        CreateForDate = 2, // Created by date
        CreateTypeEnd
    };
    bool CreateLoggerFile(uint32_t fileDateTime, CreateType createType);
    bool IsCreated(uint32_t fileDateTime, CreateType createType);
    /**
    *@brief Check and rotate to delete the early log files
    */
    void CheckAndDeleteFileNames();

    std::shared_ptr<FileHandler> CreateFileHandler(const std::string &filePath);

    void InitHandlerList();
    FileLock GetFileLock();
    void InitLoggerFiles();

    inline uint32_t GetFileDateTime(const struct tm &tmTime) const
    {
        uint32_t fileDateTime = tmTime.tm_year;
        fileDateTime <<= 4;
        fileDateTime |= tmTime.tm_mon;
        fileDateTime <<= 6;
        fileDateTime |= tmTime.tm_mday;
        return fileDateTime;
    }

    bool IsUIntReversal(const std::vector<uint32_t> &loggerIds);
    void InitLoggerIdsList(const std::vector<uint32_t> &loggerIds);

private:
    uint32_t curLogId_{0};
    long long diskFreeSizeLimit_{0};
    std::string nameTag_;
    const LoggerConfig* config_{nullptr};
    std::mutex mtx_;
    std::shared_ptr<FileHandlerList> fileHandlerList_{nullptr};
    std::list<uint32_t> loggerIds_;
    // Synchronize the log time at regular intervals
	std::chrono::steady_clock::time_point syscTimeVal_;
	// Check the log file time regularly
	std::chrono::steady_clock::time_point checkFileTimeVal_;
    // Check the disk time regularly
	std::chrono::steady_clock::time_point checkDiskTimeVal_;
};

class LoggerOutputToFileOneThread : public LoggerOutputToFile {
public:
    LoggerOutputToFileOneThread() = delete;
    LoggerOutputToFileOneThread(const std::string& tag, const LoggerConfig* config) :
        LoggerOutputToFile(tag, config) { }
    virtual ~LoggerOutputToFileOneThread() = default;

    void PreviousCheck(const std::shared_ptr<LoggerData>& loggerData)
    {
        LoggerOutputToFile::PreviousCheck(loggerData);
        Run(std::chrono::steady_clock::now(), false);
    }
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_OUTPUT_TO_FILE_H

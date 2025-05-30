#include "LoggerOutputToFile.h"

#include <iostream>
#include <stack>
#include <algorithm>
#include "FileLock.h"
#include "LoggerUtil.h"

namespace agile {
namespace logger {
LoggerOutputToFile::LoggerOutputToFile(const std::string& tag, const LoggerConfig* config) : LoggerOutputImpl(),
    nameTag_(tag), config_(config)
{
    InitHandlerList();
}

LoggerOutputToFile::~LoggerOutputToFile()
{
    FileHandlerNode* node = fileHandlerList_->Tail();
    while (node != nullptr) {
        if (!node->isFinished) {
            node->fileHandler->Flush();
            node->fileHandler->Sync();
        }
        node = node->prev;
    }
}

static FileHandlerNode gEmptyNode(0, 0, "", std::make_shared<EmptyFileHandler>());
void LoggerOutputToFile::PreviousCheck(const std::shared_ptr<LoggerData>& loggerData)
{
    FileHandlerNode* tail = fileHandlerList_->IncrRefTail();
    if (tail->fileSize >= config_->logger_file_size) {
        if (!CreateLoggerFile(0, CreateType::CreateForFileSize)) {
            loggerData->ptr = &gEmptyNode;
            return;
        }
        --tail->refs;
        tail = fileHandlerList_->IncrRefTail();
    } else {
        if (config_->logger_split_with_date) {
            uint32_t fileDateTime = GetFileDateTime(loggerData->tm_time);
            if (tail->fileDateTime < fileDateTime) {
                if (!CreateLoggerFile(fileDateTime, CreateType::CreateForDate)) {
                    loggerData->ptr = &gEmptyNode;
                    return;
                }
                --tail->refs;
                tail = fileHandlerList_->IncrRefTail();
            }
        } else {
            if (!tail->valid && !tail->isFinished) {
                if (!CreateLoggerFile(0, CreateType::CreateForInvalid)) {
                    loggerData->ptr = &gEmptyNode;
                    return;
                }
                --tail->refs;
                tail = fileHandlerList_->IncrRefTail();
            }
        }
    }
    loggerData->ptr = tail;
}

void LoggerOutputToFile::OnLoggerData(const std::shared_ptr<LoggerData>& loggerData)
{
    FileHandlerNode* node = reinterpret_cast<FileHandlerNode*>(loggerData->ptr);
    node->fileSize += loggerData->loggerBuffer->GetWriteIndex();
    static_cast<void>(node->fileHandler->Write(loggerData->loggerBuffer->data()->c_str(),
        loggerData->loggerBuffer->GetWriteIndex()));
    if (node->isFinished) {
        if (node->refs <= 1) {
            node->fileHandler->Flush();
            node->fileHandler->Sync();
        }
    }
    --node->refs;
}

void LoggerOutputToFile::Run(const std::chrono::steady_clock::time_point& curTime, bool finished)
{
    if (!config_->save_real_time) {
        const bool sysCall = (syscTimeVal_ < curTime);
        if (sysCall || finished) {
            syscTimeVal_ = curTime + std::chrono::milliseconds(config_->save_interval_msec);    
            FileHandlerNode* node = fileHandlerList_->Tail();
            if (!node->isFinished) {
                node->isFinished = finished;
                node->fileHandler->Flush();
                node->fileHandler->Sync();
            }
        } else {
            return;
        }
    }

    static constexpr uint32_t kCheckFilesInterval = 5000;
    static constexpr uint32_t kCheckDiskInterval = 30000;
    if (checkFileTimeVal_ < curTime) {
        checkFileTimeVal_ = curTime + std::chrono::milliseconds(kCheckFilesInterval);
        CheckFiles(curTime);
        if (checkDiskTimeVal_ < curTime) {
            checkDiskTimeVal_ = curTime + std::chrono::milliseconds(kCheckDiskInterval);
            CheckDisk();
        }
    }
}

bool LoggerOutputToFile::IsCreated(uint32_t fileDateTime, LoggerOutputToFile::CreateType createType)
{
    FileHandlerNode* tail = fileHandlerList_->Tail();
    if (createType == CreateType::CreateForFileSize) {
        return (tail->fileSize < config_->logger_file_size);
    } else if (createType == CreateType::CreateForFileSize) {
        return (tail->fileDateTime == fileDateTime);
    } else if (createType == CreateType::CreateForInvalid) {
        return tail->valid;
    } else {
        return false;
    }
}

std::shared_ptr<FileHandler> LoggerOutputToFile::CreateFileHandler(const std::string &filePath)
{
    if (config_->logger_file_size == 0) {
        return std::make_shared<EmptyFileHandler>();
    }
    std::shared_ptr<FileHandler> fHandler = nullptr;
    try {
        if (config_->save_real_time) {
            fHandler = std::make_shared<SyncFileHandler>(filePath);
        } else {
            if (config_->save_on_logger_num == 0) {
                fHandler = std::make_shared<AsyncFileHandler>(filePath);
            } else {
                fHandler = std::make_shared<AsyncFileHandlerSync>(filePath, config_->save_on_logger_num);
            }
        }
    } catch (const std::exception& e) {
        static_cast<void>(e);
        return nullptr;
    }
    return fHandler;
}

FileLock LoggerOutputToFile::GetFileLock()
{
    std::string filePath = config_->logger_dir_path;
    if (filePath.back() != '/') {
        filePath += "/";
    }
    filePath += config_->logger_name;
    FileLock fileLock(filePath + "_lock");
    return fileLock;
}

void LoggerOutputToFile::InitHandlerList()
{
    FileLock fileLock = GetFileLock();
    fileLock.Lock();
    InitLoggerFiles();
    const std::string filePath = LoggerUtil::GetLoggerFileName(config_, nameTag_, curLogId_, loggerIds_);
    if (filePath.empty()) {
        return;
    }
    std::shared_ptr<FileHandler> fHandler = CreateFileHandler(filePath);
    if (fHandler == nullptr) {
        exit(0);
        return;
    }
    if (curLogId_ != loggerIds_.back()) {
        loggerIds_.push_back(curLogId_);
    }

    struct tm tmTime = LoggerUtil::GetLoggerTmTime();
    uint32_t fileDateTime = GetFileDateTime(tmTime);
    FileHandlerNode* node = new FileHandlerNode(curLogId_, fileDateTime, filePath, fHandler);
    node->fileSize = LoggerUtil::GetFileSize(node->filePath);
    fileHandlerList_ = std::make_shared<FileHandlerList>();
    fileHandlerList_->EmplaceBack(node);
    CheckAndDeleteFileNames();
}

bool LoggerOutputToFile::CreateLoggerFile(uint32_t fileDateTime, LoggerOutputToFile::CreateType createType)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (IsCreated(fileDateTime, createType)) {
        return true;
    }
    FileLock fileLock = GetFileLock();
    fileLock.Lock();
    ++curLogId_;
    const std::string filePath = LoggerUtil::GetLoggerFileName(config_, nameTag_, curLogId_, loggerIds_);
    if (filePath.empty()) {
        return false;
    }

    std::shared_ptr<FileHandler> fHandler = CreateFileHandler(filePath);
    if (fHandler == nullptr) {
        return false;
    }
    loggerIds_.push_back(curLogId_);
    
    FileHandlerNode* tail = fileHandlerList_->Tail();
    tail->isFinished = true;
    tail->filePath = LoggerUtil::RenameLogingToLog(tail->filePath, true);
    tail->finishTime = std::chrono::steady_clock::now();
    tail->fileHandler->Flush();
    tail->fileHandler->Sync();
    
    FileHandlerNode* node = new FileHandlerNode(curLogId_, fileDateTime, filePath, fHandler);
    node->fileSize = LoggerUtil::GetFileSize(node->filePath);
    fileHandlerList_->EmplaceBack(node);

    constexpr uint32_t maxSizeOfHandlerList = 10U;
    uint32_t sizeLimited = std::min(config_->logger_file_number, maxSizeOfHandlerList);
    while (fileHandlerList_->size() > sizeLimited) {
        FileHandlerNode* head = fileHandlerList_->Header();
        bool expired = head->isFinished && (head->refs <= 0);
       // bool sizeLimited = (fileHandlerList_->size() > config_->logger_file_number);
        if (expired) {
            auto deleteNode = fileHandlerList_->PopFront();
            static_cast<void>(deleteNode);
        } else {
            break;
        }
    }
    CheckAndDeleteFileNames();
    return true;
}

void LoggerOutputToFile::CheckAndDeleteFileNames()
{
    while (loggerIds_.size() > config_->logger_file_number) {
        uint32_t id = loggerIds_.front();
        std::string fileName = LoggerUtil::CreateLoggerFileName(config_, nameTag_, id);
        static_cast<void>(remove(fileName.c_str()));
        fileName += "ing";
        static_cast<void>(remove(fileName.c_str()));
        loggerIds_.pop_front();
    }
}

void LoggerOutputToFile::CheckFiles(const std::chrono::steady_clock::time_point& curTime)
{
    FileHandlerNode* node = fileHandlerList_->Tail();
    if (node->isFinished) {
        return;
    }
    const uint64_t nodeFileSize = node->fileSize;
    const int64_t fileSize = LoggerUtil::GetFileSize(node->filePath);
    if (fileSize >= 0) {
        if (static_cast<uint64_t>(fileSize) < nodeFileSize) {
            if (static_cast<uint64_t>(fileSize + fileSize) < nodeFileSize) {
                node->valid = false;
                std::cerr << "file size err current size is bigger than disk file size" << std::endl;
            }
        } else {
            node->fileSize = static_cast<uint64_t>(fileSize);
        }
    }
}

void LoggerOutputToFile::CheckDisk()
{
    const long long freeSize = LoggerUtil::GetDiskFreeCapacity(config_->logger_dir_path);
    if (diskFreeSizeLimit_ == 0) {
        constexpr uint32_t diskFreeMultiVal = 10;
        diskFreeSizeLimit_ = (long long)config_->logger_buffer_size_limit * diskFreeMultiVal;
    }
    if (freeSize > diskFreeSizeLimit_) {
        return;
    }
    std::vector<std::string> delFiles;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        size_t len = (loggerIds_.size() * 2) / 3;
        while (delFiles.size() < len) {
            uint32_t id = loggerIds_.front();
            std::string fileName = LoggerUtil::CreateLoggerFileName(config_, nameTag_, id);
            delFiles.push_back(fileName);
            fileName += "ing";
            delFiles.push_back(fileName);
            loggerIds_.pop_front();
        }
    }
    for (const auto& it : delFiles) {
        remove(it.c_str());
    }
}

bool LoggerOutputToFile::IsUIntReversal(const std::vector<uint32_t> &loggerIds)
{
    if (loggerIds.size() <= 1) {
        return false;
    }
    uint32_t maxVal = UINT32_MAX - config_->logger_file_number;
    uint32_t minVal = config_->logger_file_number;
    return (loggerIds[0] < minVal && loggerIds.back() > maxVal);
}

void LoggerOutputToFile::InitLoggerIdsList(const std::vector<uint32_t> &loggerIds)
{
    if (!IsUIntReversal(loggerIds)) {
        for (auto it : loggerIds) {
            loggerIds_.push_back(it);
        }
    } else {
        std::stack<uint32_t> val;
        for (size_t i = 0; i < loggerIds.size(); i++) {
            if (loggerIds[i] < config_->logger_file_number) {
                loggerIds_.push_back(loggerIds[i]);
            } else {
                val.push(loggerIds[i]);
            }
        }
        while (!val.empty()) {
            loggerIds_.push_front(val.top());
            val.pop();
        }
    }
}

void LoggerOutputToFile::InitLoggerFiles()
{
    std::vector<std::string> filesName;
    const std::string fliter = config_->logger_name + "_" + nameTag_;
    filesName = LoggerUtil::GetLoggerFiles(config_->logger_dir_path, fliter);
    if (filesName.empty()) {
        return;
    }
    std::map<uint32_t, std::string> fileNameMap = LoggerUtil::GetLoggerOrderIds(filesName);
    std::vector<uint32_t> loggerIds;
    for (auto it = fileNameMap.begin(); it != fileNameMap.end(); it++) {
        loggerIds.push_back(it->first);
        if (LoggerUtil::IsLogingFile(it->second)) {
            static_cast<void>(LoggerUtil::RenameLogingToLog(it->second));
        }
    }
    std::sort(loggerIds.begin(), loggerIds.end());
    InitLoggerIdsList(loggerIds);
    curLogId_ = loggerIds_.back();
}
} // namespace logger
} // namespace agile

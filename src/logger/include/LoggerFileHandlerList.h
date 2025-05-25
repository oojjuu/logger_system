#ifndef AGILE_LOGGER_LOGGER_FILE_HANDLER_LIST_H
#define AGILE_LOGGER_LOGGER_FILE_HANDLER_LIST_H

#include <chrono>
#include <atomic>

#include "LoggerFileHandler.h"

namespace agile {
namespace logger {
/**
*@brief Log linked list node
*/
struct FileHandlerNode {
    FileHandlerNode() = delete;
    FileHandlerNode(uint32_t loggerId, uint32_t fileDateTime, const std::string& filePath, 
        const std::shared_ptr<FileHandler>& v): loggerId(loggerId), fileSize(0),
        fileDateTime(fileDateTime), filePath(filePath), isFinished{false}, fileHandler(v) { }
    ~FileHandlerNode() = default;

    uint32_t loggerId;
    std::atomic<uint64_t> fileSize;
    // created time
    uint32_t fileDateTime;
    std::string filePath;

    std::atomic<bool> isFinished;
    std::atomic<uint32_t> refs{0};
    bool valid = true;

    std::shared_ptr<FileHandler> fileHandler = nullptr;
    FileHandlerNode* prev = nullptr;
    FileHandlerNode* next = nullptr;
    // finished time
    std::chrono::steady_clock::time_point finishTime = std::chrono::steady_clock::now();
};

/**
*@brief Log operation linked list
*/
class FileHandlerList final {
public:
    FileHandlerList() = default;
    ~FileHandlerList();

    FileHandlerNode* Header() const { return head_; }

    FileHandlerNode* Tail() const { return tail_; }

    size_t size() const { return size_; }

    void EmplaceBack(FileHandlerNode* node);

    std::shared_ptr<FileHandlerNode> PopFront();

    FileHandlerNode* IsExistLoggerId(uint32_t loggerId);

private:
    std::atomic<size_t> size_{0};
    FileHandlerNode* head_ = nullptr;
    FileHandlerNode* tail_ = nullptr;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_FILE_HANDLER_LIST_H

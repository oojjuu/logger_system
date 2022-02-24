#pragma once

#include <chrono>
#include <atomic>
#include "LoggerFileHandler.h"

namespace agile {
namespace logger {
    
/**
*@brief 日志操作链表节点
*/
struct FileHandlerNode {
    FileHandlerNode() = delete;
    FileHandlerNode(uint32_t logger_id, uint32_t file_date_time, const std::string& file_path, 
                    const std::shared_ptr<LoggerFileHandler>& v): logger_id(logger_id), file_size(0),
                    file_date_time(file_date_time), file_path(file_path), file_handler(v) { }
    ~FileHandlerNode() = default;
    // 日志id
    uint32_t logger_id = 0;
    // file size
    std::atomic<uint64_t> file_size;
    // file date time
    uint32_t file_date_time = 0;
    // file path
    std::string file_path;
    // 是否已经完成
    bool is_finish = false;
    // 创建时间
    std::chrono::steady_clock::time_point finish_time = std::chrono::steady_clock::now();
    // 文件句柄
    std::shared_ptr<LoggerFileHandler> file_handler = nullptr;
    // 向前指针
    FileHandlerNode* prev = nullptr;
    // 向后指针
    FileHandlerNode* next = nullptr;
};

/**
*@brief 日志操作链表
*/
class FileHandlerList
{
public:
    FileHandlerList():size_(0){ }
    ~FileHandlerList();

    /**
    *@brief 获取链表头部
    *@return 链表头部指针
    */
    FileHandlerNode* header() const { return head_; }

    /**
    *@brief 获取链表尾部
    *@return 链表尾部指针
    */
    FileHandlerNode* tail() const { return tail_; }

    /**
    *@brief 获取链表长度
    *@return 链表长度
    */
    size_t size() const { return size_; }
    
    /**
    *@brief 插入链表尾部
    *@param node 节点数据
    */
    void EmplaceBack(FileHandlerNode* node);

    /**
    *@brief Pop链表头部
    *@return 链表头部智能指针
    */
    std::shared_ptr<FileHandlerNode> PopFront();

    /**
    *@brief 是否存在指定日志ID
    *@param logger_id 日志ID
    *@return FileHandlerNode*
    */
    FileHandlerNode* IsExistLoggerId(uint32_t logger_id);

private:
    // 链表长度
    std::atomic<size_t> size_;
    // 链表头部
    FileHandlerNode* head_ = nullptr;
    // 链表尾部
    FileHandlerNode* tail_ = nullptr;
};

} // namespace logger
} // namespace agile
#include "LoggerFileHandlerList.h"

namespace agile {
namespace logger {

FileHandlerList::~FileHandlerList() {
    while (head_) {
        FileHandlerNode* temp = head_;
        head_ = head_->next;
        delete temp;
        temp = nullptr;
    }
}

FileHandlerNode* FileHandlerList::IsExistLoggerId(uint32_t logger_id) {
    FileHandlerNode* node = tail_;
    while (node) {
        if (node->logger_id == logger_id) {
            return node;
        }
        node = node->prev;
    }
    return nullptr;
}

void FileHandlerList::EmplaceBack(FileHandlerNode* node) {
    ++size_;
    node->next = nullptr;
    if (tail_ == nullptr) {
        node->prev = nullptr;
        tail_ = head_ = node;
        return;
    }
    tail_->next = node;
    node->prev = tail_;
    tail_ = node;
}

std::shared_ptr<FileHandlerNode> FileHandlerList::PopFront() {
    if (!head_) {
        return nullptr;
    }
    auto res = head_;
    head_ = head_->next;
    if (head_) {
        head_->prev = nullptr;
    }
    else {
        tail_ = head_;
    }
    --size_;
    return std::shared_ptr<FileHandlerNode>(res);
}
    
} // namespace logger
} // namespace agile
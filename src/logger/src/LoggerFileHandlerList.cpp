#include "LoggerFileHandlerList.h"

namespace agile {
namespace logger {
FileHandlerList::~FileHandlerList()
{
    while (head_ != nullptr) {
        FileHandlerNode* del = head_;
        head_ = head_->next;
        delete del;
        del = nullptr;
    }
}

FileHandlerNode* FileHandlerList::IsExistLoggerId(uint32_t loggerId)
{
    FileHandlerNode* node = tail_;
    while (node != nullptr) {
        if (node->loggerId == loggerId) {
            return node;
        }
        node = node->prev;
    }
    return nullptr;
}

void FileHandlerList::EmplaceBack(FileHandlerNode* node)
{
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

std::shared_ptr<FileHandlerNode> FileHandlerList::PopFront()
{
    if (head_ == nullptr) {
        return nullptr;
    }
    auto res = head_;
    head_ = head_->next;
    if (head_ != nullptr) {
        head_->prev = nullptr;
    } else {
        tail_ = head_;
    }
    --size_;
    return std::shared_ptr<FileHandlerNode>(res);
}
} // namespace logger
} // namespace agile

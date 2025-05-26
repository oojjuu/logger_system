#ifndef AGILE_MULTI_THREAD_LIST_H
#define AGILE_MULTI_THREAD_LIST_H

#include <list>
#include <memory>
#include <mutex>

#include "LoggerData.h"
#include "ThreadLock.h"

namespace agile {
namespace logger {
template<typename T>
class MultiThreadList {
public:
    MultiThreadList();
    MultiThreadList(size_t maxSize);
    virtual ~MultiThreadList() = default;

    virtual bool PushBack(const std::shared_ptr<T> &val);
    virtual std::shared_ptr<T> PopBack();
    virtual void Swap(std::list<std::shared_ptr<T>> &val);

protected:
    ILock *lock_{nullptr};

private:
    MutexLock mtx_;
    size_t maxSize_{0};
    std::list<std::shared_ptr<T>> list_;
};

template<typename T>
MultiThreadList<T>::MultiThreadList()
{
    lock_ = &mtx_;
}

template<typename T>
MultiThreadList<T>::MultiThreadList(size_t maxSize)
{
    maxSize_ = maxSize;
    lock_ = &mtx_;
}

template<typename T>
bool MultiThreadList<T>::PushBack(const std::shared_ptr<T> &val)
{
    LockGuard guard{lock_};
    if (MultiThreadList<T>::maxSize_ != 0 &&
        MultiThreadList<T>::list_.size() >= MultiThreadList<T>::maxSize_) {
        return false;
    }
    MultiThreadList<T>::list_.push_back(val);
    return true;
}

template<typename T>
std::shared_ptr<T> MultiThreadList<T>::PopBack()
{
    LockGuard guard{lock_};
    if (MultiThreadList<T>::list_.empty()) {
        return nullptr;
    }
    std::shared_ptr<T> res = MultiThreadList<T>::list_.front();
    MultiThreadList<T>::list_.pop_front();
    return res;
}

template<typename T>
void MultiThreadList<T>::Swap(std::list<std::shared_ptr<T>> &val)
{
    LockGuard guard{lock_};
    MultiThreadList<T>::list_.swap(val);
}

template<class T>
class QuickThreadList : public MultiThreadList<T> {
public:
    QuickThreadList()
    {
        MultiThreadList<T>::lock_ = &spinLock_;
    }
    QuickThreadList(size_t maxSize) : MultiThreadList<T>(maxSize) 
    {
        MultiThreadList<T>::lock_ = &spinLock_;
    }
    ~QuickThreadList() = default;

private:
    SpinLock spinLock_;
};
} // namespace logger
} // namespace agile
#endif // AGILE_MULTI_THREAD_LIST_H

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
    MultiThreadList() = default;
    MultiThreadList(size_t maxSize) : maxSize_{maxSize} { }
    virtual ~MultiThreadList() = default;

    bool PushBack(const std::shared_ptr<T> &val);
    std::shared_ptr<T> PopBack();
    void Swap(std::list<std::shared_ptr<T>> &val);

protected:
    virtual void InitLock() = 0;
    ILock *lock_ = nullptr; // lock_ value can not be nullptr

private:
    size_t maxSize_{0};
    std::list<std::shared_ptr<T>> list_;
};

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
class MutexThreadList : public MultiThreadList<T> {
public:
    MutexThreadList()
    {
        InitLock();
    }
    MutexThreadList(size_t maxSize) : MultiThreadList<T>(maxSize)
    {
        InitLock();
    }
    ~MutexThreadList() = default;

private:
    void InitLock()
    {
        MultiThreadList<T>::lock_ = &mtx_;
    }
    MutexLock mtx_;
};

template<class T>
class QuickThreadList : public MultiThreadList<T> {
public:
    QuickThreadList()
    {
        InitLock();
    }
    QuickThreadList(size_t maxSize) : MultiThreadList<T>(maxSize)
    {
        InitLock();
    }
    ~QuickThreadList() = default;

private:
    void InitLock()
    {
        MultiThreadList<T>::lock_ = &spinLock_;
    }
    SpinLock spinLock_;
};
} // namespace logger
} // namespace agile
#endif // AGILE_MULTI_THREAD_LIST_H

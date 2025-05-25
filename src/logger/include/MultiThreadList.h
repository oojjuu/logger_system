#ifndef AGILE_MULTI_THREAD_LIST_H
#define AGILE_MULTI_THREAD_LIST_H

#include <list>
#include <memory>
#include <mutex>

#include "LoggerData.h"
#include "ThreadLock.h"

namespace agile {
namespace logger {
template<class T>
class MultiThreadList {
public:
    MultiThreadList() = default;
    MultiThreadList(size_t maxSize) : maxSize_{maxSize} { }
    virtual ~MultiThreadList() = default;
	
    virtual bool PushBack(const std::shared_ptr<T> &val) = 0;
    virtual void PushBackUnlimited(const std::shared_ptr<T> &val) = 0;
    virtual std::shared_ptr<T> PopBack() = 0;
    virtual void Swap(std::list<std::shared_ptr<T>> &val) = 0;
    size_t GetSize() const { return size_; }

protected:
    size_t size_{0};
    size_t maxSize_{0};
    std::list<std::shared_ptr<T>> list_;
};

template<class T>
class QuickThreadList : public MultiThreadList<T> {
public:
    QuickThreadList() = default;
    QuickThreadList(size_t maxSize) : MultiThreadList<T>(maxSize) { }
    ~QuickThreadList() = default;

    bool PushBack(const std::shared_ptr<T> &val);
    void PushBackUnlimited(const std::shared_ptr<T> &val);
    std::shared_ptr<T> PopBack();
    void Swap(std::list<std::shared_ptr<T>> &val);

private:
    SpinLock lock_;
};

template<typename T>
bool QuickThreadList<T>::PushBack(const std::shared_ptr<T> &val)
{   
    lock_.Lock();
    if (MultiThreadList<T>::size_ >= MultiThreadList<T>::maxSize_) {
        lock_.UnLock();
        return false;
    }
    MultiThreadList<T>::list_.push_back(val);
    ++MultiThreadList<T>::size_;
    lock_.UnLock();
    return true;
}

template<typename T>
void QuickThreadList<T>::PushBackUnlimited(const std::shared_ptr<T> &val)
{
    lock_.Lock();
    MultiThreadList<T>::list_.push_back(val);
    ++MultiThreadList<T>::size_;
    lock_.UnLock();
}

template<typename T>
std::shared_ptr<T> QuickThreadList<T>::PopBack()
{
    lock_.Lock();
    if (MultiThreadList<T>::list_.empty()) {
        lock_.UnLock();
        return nullptr;
    }
    std::shared_ptr<T> res = MultiThreadList<T>::list_.front();
    --MultiThreadList<T>::size_;
    MultiThreadList<T>::list_.pop_front();
    lock_.UnLock();
    return res;
}

template<typename T>
void QuickThreadList<T>::Swap(std::list<std::shared_ptr<T>> &val)
{
    lock_.Lock();
    MultiThreadList<T>::list_.swap(val);
    MultiThreadList<T>::size_ = MultiThreadList<T>::list_.size();
    lock_.UnLock();
}

template<typename T>
class SlowThreadList : public MultiThreadList<T> {
public:
    SlowThreadList() = default;
    SlowThreadList(size_t maxSize) : MultiThreadList<T>(maxSize) { }
    ~SlowThreadList() = default;

    bool PushBack(const std::shared_ptr<T> &val);
    void PushBackUnlimited(const std::shared_ptr<T> &val);
    std::shared_ptr<T> PopBack();
    void Swap(std::list<std::shared_ptr<T>> &val);

private:
    std::mutex mtx_;
};

template<typename T>
bool SlowThreadList<T>::PushBack(const std::shared_ptr<T> &val)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (MultiThreadList<T>::size_ >= MultiThreadList<T>::maxSize_) {
        return false;
    }
    MultiThreadList<T>::list_.push_back(val);
    ++MultiThreadList<T>::size_;
    return true;
}

template<typename T>
void SlowThreadList<T>::PushBackUnlimited(const std::shared_ptr<T> &val)
{
    std::lock_guard<std::mutex> lock(mtx_);
    MultiThreadList<T>::list_.push_back(val);
    ++MultiThreadList<T>::size_;
}

template<typename T>
std::shared_ptr<T> SlowThreadList<T>::PopBack()
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (MultiThreadList<T>::list_.empty()) {
        return nullptr;
    }
    std::shared_ptr<T> res = MultiThreadList<T>::list_.front();
    MultiThreadList<T>::list_.pop_front();
    --MultiThreadList<T>::size_;
    return res;
}

template<typename T>
void SlowThreadList<T>::Swap(std::list<std::shared_ptr<T>> &val)
{
    std::lock_guard<std::mutex> lock(mtx_);
    MultiThreadList<T>::list_.swap(val);
    MultiThreadList<T>::size_ = MultiThreadList<T>::list_.size();
}
} // namespace logger
} // namespace agile
#endif // AGILE_MULTI_THREAD_LIST_H

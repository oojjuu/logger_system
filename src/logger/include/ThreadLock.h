#ifndef AGILE_THREAD_LOCK_H
#define AGILE_THREAD_LOCK_H

#include <atomic>
#include <mutex>

namespace agile {
namespace logger {
class ILock {
public:
    ILock() = default;
    virtual ~ILock() = default;

    virtual void Lock() = 0;
    virtual void UnLock() = 0;
};

class SpinLock final : public ILock {
public:
    SpinLock() = default;
    ~SpinLock() = default;
	
	void Lock();
    void UnLock();

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

class MutexLock final : public ILock {
public:
    MutexLock() = default;
    ~MutexLock() = default;
    
    void Lock();
    void UnLock();

private:
    std::mutex mtx_;
};

class LockGuard final {
public:
    LockGuard() = delete;
    // lock value can not be nullptr
    LockGuard(ILock *lock);
    ~LockGuard();

private:
    ILock *lock_;
};

template<typename T>
class LockGuardRef final {
public:
    LockGuardRef() = delete;
    LockGuardRef(T &lock) : lock_{lock}
    {
        lock_.Lock();
    }
    ~LockGuardRef()
    {
        lock_.UnLock();
    }

private:
    T &lock_;
};
} // namespace logger
} // namespace agile
#endif // AGILE_THREAD_LOCK_H

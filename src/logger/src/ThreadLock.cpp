#include "ThreadLock.h"

namespace agile {
namespace logger {
void SpinLock::Lock()
{
    while (flag_.test_and_set(std::memory_order_acquire)) {
    }
}

void SpinLock::UnLock()
{
    flag_.clear(std::memory_order_release);
}

void MutexLock::Lock()
{
    mtx_.lock();
}

void MutexLock::UnLock()
{
    mtx_.unlock();
}

LockGuard::LockGuard(ILock *lock) : lock_{lock}
{
    lock_->Lock();
}

LockGuard::~LockGuard()
{
    lock_->UnLock();
}
} // logger
} // agile
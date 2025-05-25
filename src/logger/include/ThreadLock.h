#ifndef AGILE_THREAD_LOCK_H
#define AGILE_THREAD_LOCK_H

#include <atomic>

namespace agile {
namespace logger {
class SpinLock final {
public:
    SpinLock() = default;
    ~SpinLock() = default;
	
	void Lock();
    void UnLock();

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};
} // namespace logger
} // namespace agile
#endif // AGILE_THREAD_LOCK_H

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
} // logger
} // agile
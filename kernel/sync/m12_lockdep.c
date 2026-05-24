#include <mcsos/sync/mcs_sync.h>

static uint32_t m12_lock_depth;

void m12_lockdep_acquire(void)
{
    ++m12_lock_depth;
}

void m12_lockdep_release(void)
{
    if (m12_lock_depth > 0) {
        --m12_lock_depth;
    }
}

uint32_t m12_lockdep_depth(void)
{
    return m12_lock_depth;
}

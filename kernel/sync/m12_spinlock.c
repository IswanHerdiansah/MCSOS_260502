#include <mcsos/sync/mcs_sync.h>

static inline void mcs_cpu_pause(void)
{
    __asm__ volatile("pause");
}

void mcs_spin_init(
    struct mcs_spinlock *lock
) {
    lock->locked = 0;
}

int mcs_spin_try_lock(
    struct mcs_spinlock *lock
) {
    return __sync_lock_test_and_set(
        &lock->locked,
        1
    ) == 0;
}

void mcs_spin_lock(
    struct mcs_spinlock *lock
) {
    while (!mcs_spin_try_lock(lock)) {
        mcs_cpu_pause();
    }
}

void mcs_spin_unlock(
    struct mcs_spinlock *lock
) {
    __sync_lock_release(
        &lock->locked
    );
}

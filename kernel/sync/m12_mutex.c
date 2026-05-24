#include <mcsos/sync/mcs_sync.h>

void mcs_mutex_init(
    struct mcs_mutex *mutex
) {
    mutex->locked = 0;
}

void mcs_mutex_lock(
    struct mcs_mutex *mutex
) {
    while (__sync_lock_test_and_set(
        &mutex->locked,
        1
    )) {
    }
}

void mcs_mutex_unlock(
    struct mcs_mutex *mutex
) {
    __sync_lock_release(
        &mutex->locked
    );
}

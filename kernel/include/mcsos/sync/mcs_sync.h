#ifndef MCSOS_SYNC_MCS_SYNC_H
#define MCSOS_SYNC_MCS_SYNC_H

#include <stdint.h>

struct mcs_spinlock {
    volatile uint32_t locked;
};

struct mcs_mutex {
    volatile uint32_t locked;
};

void mcs_spin_init(
    struct mcs_spinlock *lock
);

int mcs_spin_try_lock(
    struct mcs_spinlock *lock
);

void mcs_spin_lock(
    struct mcs_spinlock *lock
);

void mcs_spin_unlock(
    struct mcs_spinlock *lock
);

void mcs_mutex_init(
    struct mcs_mutex *mutex
);

void mcs_mutex_lock(
    struct mcs_mutex *mutex
);

void mcs_mutex_unlock(
    struct mcs_mutex *mutex
);

void m12_lockdep_acquire(void);

void m12_lockdep_release(void);

uint32_t m12_lockdep_depth(void);

#endif

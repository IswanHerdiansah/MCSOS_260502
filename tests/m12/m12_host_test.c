#include <assert.h>
#include <stdio.h>

#include <mcsos/sync/mcs_sync.h>

int main(void)
{
    struct mcs_spinlock spin;

    struct mcs_mutex mutex;

    mcs_spin_init(&spin);

    assert(spin.locked == 0);

    mcs_spin_lock(&spin);

    assert(spin.locked == 1);

    mcs_spin_unlock(&spin);

    assert(spin.locked == 0);

    printf(
        "PASS spinlock acquire/release\n"
    );

    mcs_mutex_init(&mutex);

    assert(mutex.locked == 0);

    mcs_mutex_lock(&mutex);

    assert(mutex.locked == 1);

    mcs_mutex_unlock(&mutex);

    assert(mutex.locked == 0);

    printf(
        "PASS mutex acquire/release\n"
    );

    m12_lockdep_acquire();

    assert(m12_lockdep_depth() == 1);

    m12_lockdep_release();

    assert(m12_lockdep_depth() == 0);

    printf(
        "PASS lockdep depth tracking\n"
    );

    puts("[PASS] M12 synchronization host tests passed.");

    return 0;
}

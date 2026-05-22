#include <stdint.h>
#include <stddef.h>

#include <mcsos/kernel/syscall.h>

static uint64_t
sys_ping(void) {

    return 0x2605020Au;
}

static uint64_t
sys_get_ticks(void) {

    return 800u;
}

static uint64_t
sys_write_serial(
    const char *buf,
    uint64_t len
) {

    if (buf == 0) {
        return MCSOS_EFAULT;
    }

    return len;
}

static uint64_t
sys_yield(void) {

    return 0;
}

static uint64_t
sys_exit_thread(
    uint64_t code
) {

    (void) code;

    return 0;
}

uint64_t
mcsos_syscall_dispatch(
    uint64_t nr,
    uint64_t arg0,
    uint64_t arg1,
    uint64_t arg2,
    uint64_t arg3,
    uint64_t arg4,
    uint64_t arg5
) {

    (void) arg2;
    (void) arg3;
    (void) arg4;
    (void) arg5;

    switch (nr) {

        case MCSOS_SYS_PING:
            return sys_ping();

        case MCSOS_SYS_GET_TICKS:
            return sys_get_ticks();

        case MCSOS_SYS_WRITE_SERIAL:
            return sys_write_serial(
                (const char *) arg0,
                arg1
            );

        case MCSOS_SYS_YIELD:
            return sys_yield();

        case MCSOS_SYS_EXIT_THREAD:
            return sys_exit_thread(arg0);

        default:
            return MCSOS_ENOSYS;
    }
}

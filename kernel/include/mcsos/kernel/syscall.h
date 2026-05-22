#ifndef MCSOS_KERNEL_SYSCALL_H
#define MCSOS_KERNEL_SYSCALL_H

#include <stdint.h>
#include <stddef.h>

#define MCSOS_SYS_PING          0u
#define MCSOS_SYS_GET_TICKS     1u
#define MCSOS_SYS_WRITE_SERIAL  2u
#define MCSOS_SYS_YIELD         3u
#define MCSOS_SYS_EXIT_THREAD   4u

#define MCSOS_SYS_MAX           5u

#define MCSOS_ENOSYS  (-38)
#define MCSOS_EINVAL  (-22)
#define MCSOS_EFAULT  (-14)
#define MCSOS_EBUSY   (-16)

uint64_t
mcsos_syscall_dispatch(
    uint64_t nr,
    uint64_t arg0,
    uint64_t arg1,
    uint64_t arg2,
    uint64_t arg3,
    uint64_t arg4,
    uint64_t arg5
);

#endif

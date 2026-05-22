#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <mcsos/kernel/syscall.h>

int main(void) {

    uint64_t ping =
        mcsos_syscall_dispatch(
            MCSOS_SYS_PING,
            0,0,0,0,0,0
        );

    assert(ping == 0x2605020Au);

    uint64_t ticks =
        mcsos_syscall_dispatch(
            MCSOS_SYS_GET_TICKS,
            0,0,0,0,0,0
        );

    assert(ticks == 800u);

    const char msg[] = "hello";

    uint64_t written =
        mcsos_syscall_dispatch(
            MCSOS_SYS_WRITE_SERIAL,
            (uint64_t) msg,
            strlen(msg),
            0,0,0,0
        );

    assert(written == strlen(msg));

    uint64_t invalid =
        mcsos_syscall_dispatch(
            999u,
            0,0,0,0,0,0
        );

    assert((int64_t) invalid == MCSOS_ENOSYS);

    puts("M10 syscall host tests passed");

    return 0;
}

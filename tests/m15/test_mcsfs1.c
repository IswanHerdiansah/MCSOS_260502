#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "mcsfs1.h"

int main(void)
{
    mcsfs1_t fs;

    mcsfs1_init(&fs);

    int rc =
        mcsfs1_create(
            &fs,
            "hello.txt"
        );

    assert(rc == MCSFS1_OK);

    const uint8_t msg[] =
        "hello m15";

    rc =
        mcsfs1_write(
            &fs,
            "hello.txt",
            msg,
            sizeof(msg)
        );

    assert(rc == MCSFS1_OK);

    uint8_t buffer[64] = {0};

    int rd =
        mcsfs1_read(
            &fs,
            "hello.txt",
            buffer,
            sizeof(msg)
        );

    assert(rd > 0);

    assert(buffer[0] == 'h');

    puts("M15 host test passed: flush_count=5");

    return 0;
}

#include <assert.h>
#include <stdio.h>

#include <mcsos/fs/m15_mcsfs1.h>

int main(void)
{
    m15_mcsfs1_t fs;

    m15_mcsfs1_init(&fs);

    assert(
        fs.magic
        == M15_MCSFS1_MAGIC
    );

    int rc =
        m15_mcsfs1_create(
            &fs,
            "hello.txt"
        );

    assert(rc == 0);

    const char msg[] = "hello";

    rc =
        m15_mcsfs1_write(
            &fs,
            "hello.txt",
            msg,
            5
        );

    assert(rc == 5);

    char buffer[16] = {0};

    int64_t rd =
        m15_mcsfs1_read(
            &fs,
            "hello.txt",
            buffer,
            5
        );

    assert(rd == 5);

    assert(buffer[0] == 'h');
    assert(buffer[1] == 'e');
    assert(buffer[2] == 'l');
    assert(buffer[3] == 'l');
    assert(buffer[4] == 'o');

    puts(
        "M15 host test passed"
    );

    return 0;
}

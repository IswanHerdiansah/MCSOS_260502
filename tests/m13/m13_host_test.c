#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <mcsos/vfs/mcs_vfs.h>

int main(void)
{
    mcs_ramfs_t fs;

    mcs_fd_table_t table;

    mcs_ramfs_init(&fs);

    mcs_fd_table_init(&table);

    const char hello[] = "hello";

    int seed =
        mcs_ramfs_seed_file(
            &fs,
            "/hello.txt",
            (const uint8_t *)hello,
            5
        );

    assert(seed == MCS_OK);

    int fd =
        mcs_vfs_open(
            &table,
            &fs,
            "/hello.txt",
            MCS_O_RDONLY
        );

    assert(fd >= 0);

    char buffer[16] = {0};

    int64_t rd =
        mcs_vfs_read(
            &table,
            fd,
            buffer,
            5
        );

    assert(rd == 5);

    assert(buffer[0] == 'h');
    assert(buffer[1] == 'e');
    assert(buffer[2] == 'l');
    assert(buffer[3] == 'l');
    assert(buffer[4] == 'o');

    int rc =
        mcs_vfs_close(
            &table,
            fd
        );

    assert(rc == MCS_OK);

    puts(
        "M13 VFS/FD/RAMFS host tests: PASS."
    );

    return 0;
}

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <mcsos/block/mcs_block.h>

#define TEST_BLOCKS 16u

static uint8_t g_storage[
    TEST_BLOCKS * MCS_BLOCK_SIZE
];

int main(void)
{
    mcs_ram_block_device_t ramdev;

    mcs_block_device_t dev;

    mcs_ram_block_init(
        &ramdev,
        g_storage,
        TEST_BLOCKS
    );

    mcs_block_bind_ramdev(
        &dev,
        &ramdev
    );

    uint8_t write_buf[
        MCS_BLOCK_SIZE
    ];

    uint8_t read_buf[
        MCS_BLOCK_SIZE
    ];

    for (uint64_t i = 0;
         i < MCS_BLOCK_SIZE;
         ++i) {

        write_buf[i] =
            (uint8_t)(i & 0xffu);

        read_buf[i] = 0;
    }

    int wr =
        mcs_block_write(
            &dev,
            2,
            write_buf
        );

    assert(
        wr == MCS_BLOCK_OK
    );

    int rd =
        mcs_block_read(
            &dev,
            2,
            read_buf
        );

    assert(
        rd == MCS_BLOCK_OK
    );

    for (uint64_t i = 0;
         i < MCS_BLOCK_SIZE;
         ++i) {

        assert(
            read_buf[i]
            == write_buf[i]
        );
    }

    int bad =
        mcs_block_read(
            &dev,
            TEST_BLOCKS + 1,
            read_buf
        );

    assert(
        bad == MCS_BLOCK_EINVAL
    );

    puts(
        "M14 host tests PASS"
    );

    return 0;
}

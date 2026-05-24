#ifndef MCSOS_BLOCK_MCS_BLOCK_H
#define MCSOS_BLOCK_MCS_BLOCK_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MCS_BLOCK_SIZE 512u

#define MCS_BLOCK_OK        0
#define MCS_BLOCK_EINVAL   -1
#define MCS_BLOCK_EIO      -2
#define MCS_BLOCK_ENOSPC   -3

typedef struct mcs_block_device {

    uint64_t block_count;

    int (*read)(
        struct mcs_block_device *dev,
        uint64_t lba,
        void *buffer
    );

    int (*write)(
        struct mcs_block_device *dev,
        uint64_t lba,
        const void *buffer
    );

    void *driver_data;

} mcs_block_device_t;

typedef struct {

    uint8_t *storage;

    uint64_t total_blocks;

} mcs_ram_block_device_t;

void mcs_ram_block_init(
    mcs_ram_block_device_t *ramdev,
    uint8_t *storage,
    uint64_t total_blocks
);

void mcs_block_bind_ramdev(
    mcs_block_device_t *dev,
    mcs_ram_block_device_t *ramdev
);

int mcs_block_read(
    mcs_block_device_t *dev,
    uint64_t lba,
    void *buffer
);

int mcs_block_write(
    mcs_block_device_t *dev,
    uint64_t lba,
    const void *buffer
);

#endif

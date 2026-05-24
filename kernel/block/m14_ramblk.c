#include <mcsos/block/mcs_block.h>

#include <stdint.h>

static void m14_memcpy(
    uint8_t *dst,
    const uint8_t *src,
    uint64_t size
) {
    for (uint64_t i = 0;
         i < size;
         ++i) {

        dst[i] = src[i];
    }
}

static int m14_ramblk_read(
    mcs_block_device_t *dev,
    uint64_t lba,
    void *buffer
) {
    if (dev == 0 || buffer == 0) {
        return MCS_BLOCK_EINVAL;
    }

    mcs_ram_block_device_t *ramdev =
        (mcs_ram_block_device_t *)
            dev->driver_data;

    if (lba >= ramdev->total_blocks) {
        return MCS_BLOCK_EINVAL;
    }

    uint8_t *dst =
        (uint8_t *)buffer;

    uint8_t *src =
        &ramdev->storage[
            lba * MCS_BLOCK_SIZE
        ];

    m14_memcpy(
        dst,
        src,
        MCS_BLOCK_SIZE
    );

    return MCS_BLOCK_OK;
}

static int m14_ramblk_write(
    mcs_block_device_t *dev,
    uint64_t lba,
    const void *buffer
) {
    if (dev == 0 || buffer == 0) {
        return MCS_BLOCK_EINVAL;
    }

    mcs_ram_block_device_t *ramdev =
        (mcs_ram_block_device_t *)
            dev->driver_data;

    if (lba >= ramdev->total_blocks) {
        return MCS_BLOCK_EINVAL;
    }

    uint8_t *dst =
        &ramdev->storage[
            lba * MCS_BLOCK_SIZE
        ];

    const uint8_t *src =
        (const uint8_t *)buffer;

    m14_memcpy(
        dst,
        src,
        MCS_BLOCK_SIZE
    );

    return MCS_BLOCK_OK;
}

void mcs_ram_block_init(
    mcs_ram_block_device_t *ramdev,
    uint8_t *storage,
    uint64_t total_blocks
) {
    ramdev->storage = storage;

    ramdev->total_blocks =
        total_blocks;
}

void mcs_block_bind_ramdev(
    mcs_block_device_t *dev,
    mcs_ram_block_device_t *ramdev
) {
    dev->block_count =
        ramdev->total_blocks;

    dev->read =
        m14_ramblk_read;

    dev->write =
        m14_ramblk_write;

    dev->driver_data =
        ramdev;
}

int mcs_block_read(
    mcs_block_device_t *dev,
    uint64_t lba,
    void *buffer
) {
    return dev->read(
        dev,
        lba,
        buffer
    );
}

int mcs_block_write(
    mcs_block_device_t *dev,
    uint64_t lba,
    const void *buffer
) {
    return dev->write(
        dev,
        lba,
        buffer
    );
}

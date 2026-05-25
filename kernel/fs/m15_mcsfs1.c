#include <mcsos/fs/m15_mcsfs1.h>

static int m15_strcmp(
    const char *a,
    const char *b
) {
    while (*a && *b) {

        if (*a != *b) {
            return (int)(*a - *b);
        }

        ++a;
        ++b;
    }

    return (int)(*a - *b);
}

static void m15_strcpy(
    char *dst,
    const char *src
) {
    while (*src) {

        *dst++ = *src++;
    }

    *dst = '\0';
}

static void m15_memcpy(
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

static m15_mcsfs1_inode_t *
m15_find_inode(
    m15_mcsfs1_t *fs,
    const char *name
) {
    for (uint64_t i = 0;
         i < fs->inode_count;
         ++i) {

        if (fs->inodes[i].used
            && m15_strcmp(
                fs->inodes[i].name,
                name
            ) == 0) {

            return &fs->inodes[i];
        }
    }

    return 0;
}

void m15_mcsfs1_init(
    m15_mcsfs1_t *fs
) {
    fs->magic =
        M15_MCSFS1_MAGIC;

    fs->inode_count = 0;

    fs->data_used = 0;

    for (uint64_t i = 0;
         i < M15_MCSFS1_MAX_FILES;
         ++i) {

        fs->inodes[i].used = 0;
    }
}

int m15_mcsfs1_create(
    m15_mcsfs1_t *fs,
    const char *name
) {
    if (fs == 0 || name == 0) {
        return M15_MCSFS1_EINVAL;
    }

    if (fs->inode_count
        >= M15_MCSFS1_MAX_FILES) {

        return M15_MCSFS1_ENOSPC;
    }

    m15_mcsfs1_inode_t *inode =
        &fs->inodes[
            fs->inode_count
        ];

    inode->used = 1;

    inode->offset =
        fs->data_used;

    inode->size = 0;

    m15_strcpy(
        inode->name,
        name
    );

    ++fs->inode_count;

    return M15_MCSFS1_OK;
}

int m15_mcsfs1_write(
    m15_mcsfs1_t *fs,
    const char *name,
    const void *buffer,
    uint64_t size
) {
    m15_mcsfs1_inode_t *inode =
        m15_find_inode(
            fs,
            name
        );

    if (inode == 0) {
        return M15_MCSFS1_ENOENT;
    }

    if ((inode->offset + size)
        >= M15_MCSFS1_DATA_SIZE) {

        return M15_MCSFS1_ENOSPC;
    }

    m15_memcpy(
        &fs->data[inode->offset],
        (const uint8_t *)buffer,
        size
    );

    inode->size = size;

    fs->data_used =
        inode->offset + size;

    return (int)size;
}

int64_t m15_mcsfs1_read(
    m15_mcsfs1_t *fs,
    const char *name,
    void *buffer,
    uint64_t size
) {
    m15_mcsfs1_inode_t *inode =
        m15_find_inode(
            fs,
            name
        );

    if (inode == 0) {
        return M15_MCSFS1_ENOENT;
    }

    if (size > inode->size) {
        size = inode->size;
    }

    m15_memcpy(
        (uint8_t *)buffer,
        &fs->data[inode->offset],
        size
    );

    return (int64_t)size;
}

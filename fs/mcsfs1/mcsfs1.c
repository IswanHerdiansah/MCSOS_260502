#include "mcsfs1.h"

#include <stddef.h>

static uint64_t m15_strlen(
    const char *s
) {
    uint64_t len = 0;

    while (s[len] != '\0') {
        ++len;
    }

    return len;
}

static int m15_streq(
    const char *a,
    const char *b
) {
    uint64_t i = 0;

    while (a[i] != '\0'
        && b[i] != '\0') {

        if (a[i] != b[i]) {
            return 0;
        }

        ++i;
    }

    return a[i] == b[i];
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

void mcsfs1_init(
    mcsfs1_t *fs
) {
    fs->magic = MCSFS1_MAGIC;

    fs->inode_count = 0;

    for (uint64_t i = 0;
         i < MCSFS1_MAX_FILES;
         ++i) {

        fs->inodes[i].used = 0;
    }
}

int mcsfs1_create(
    mcsfs1_t *fs,
    const char *name
) {
    if (fs == NULL || name == NULL) {
        return MCSFS1_EINVAL;
    }

    if (m15_strlen(name)
        >= MCSFS1_NAME_LEN) {

        return MCSFS1_EINVAL;
    }

    for (uint64_t i = 0;
         i < MCSFS1_MAX_FILES;
         ++i) {

        if (!fs->inodes[i].used) {

            fs->inodes[i].used = 1;

            for (uint64_t j = 0;
                 j < MCSFS1_NAME_LEN;
                 ++j) {

                fs->inodes[i].name[j] =
                    name[j];

                if (name[j] == '\0') {
                    break;
                }
            }

            fs->inodes[i].size = 0;

            ++fs->inode_count;

            return MCSFS1_OK;
        }
    }

    return MCSFS1_ENOSPC;
}

int mcsfs1_write(
    mcsfs1_t *fs,
    const char *name,
    const uint8_t *data,
    uint64_t size
) {
    if (size > MCSFS1_DATA_SIZE) {
        return MCSFS1_ENOSPC;
    }

    for (uint64_t i = 0;
         i < MCSFS1_MAX_FILES;
         ++i) {

        if (fs->inodes[i].used
            && m15_streq(
                fs->inodes[i].name,
                name
            )) {

            m15_memcpy(
                fs->inodes[i].data,
                data,
                size
            );

            fs->inodes[i].size =
                size;

            return MCSFS1_OK;
        }
    }

    return MCSFS1_ENOENT;
}

int mcsfs1_read(
    mcsfs1_t *fs,
    const char *name,
    uint8_t *buffer,
    uint64_t size
) {
    for (uint64_t i = 0;
         i < MCSFS1_MAX_FILES;
         ++i) {

        if (fs->inodes[i].used
            && m15_streq(
                fs->inodes[i].name,
                name
            )) {

            if (size
                > fs->inodes[i].size) {

                size =
                    fs->inodes[i].size;
            }

            m15_memcpy(
                buffer,
                fs->inodes[i].data,
                size
            );

            return (int)size;
        }
    }

    return MCSFS1_ENOENT;
}

#ifndef MCSFS1_H
#define MCSFS1_H

#include <stdint.h>

#define MCSFS1_MAGIC 0x4D435331u

#define MCSFS1_OK        0
#define MCSFS1_EINVAL   -1
#define MCSFS1_ENOSPC   -2
#define MCSFS1_ENOENT   -3

#define MCSFS1_MAX_FILES 32
#define MCSFS1_NAME_LEN  32
#define MCSFS1_DATA_SIZE 256

typedef struct {

    uint8_t used;

    char name[MCSFS1_NAME_LEN];

    uint64_t size;

    uint8_t data[MCSFS1_DATA_SIZE];

} mcsfs1_inode_t;

typedef struct {

    uint32_t magic;

    uint32_t inode_count;

    mcsfs1_inode_t inodes[MCSFS1_MAX_FILES];

} mcsfs1_t;

void mcsfs1_init(
    mcsfs1_t *fs
);

int mcsfs1_create(
    mcsfs1_t *fs,
    const char *name
);

int mcsfs1_write(
    mcsfs1_t *fs,
    const char *name,
    const uint8_t *data,
    uint64_t size
);

int mcsfs1_read(
    mcsfs1_t *fs,
    const char *name,
    uint8_t *buffer,
    uint64_t size
);

#endif

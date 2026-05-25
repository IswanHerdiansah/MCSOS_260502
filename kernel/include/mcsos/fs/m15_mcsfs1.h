#ifndef MCSOS_FS_M15_MCSFS1_H
#define MCSOS_FS_M15_MCSFS1_H

#include <stdint.h>

#define M15_MCSFS1_MAGIC 0x4D4353465331ULL

#define M15_MCSFS1_OK       0
#define M15_MCSFS1_EINVAL  -1
#define M15_MCSFS1_ENOENT  -2
#define M15_MCSFS1_ENOSPC  -3

#define M15_MCSFS1_MAX_FILES 32
#define M15_MCSFS1_NAME_LEN  64
#define M15_MCSFS1_DATA_SIZE 4096

typedef struct {

    char name[M15_MCSFS1_NAME_LEN];

    uint64_t offset;

    uint64_t size;

    uint8_t used;

} m15_mcsfs1_inode_t;

typedef struct {

    uint64_t magic;

    uint64_t inode_count;

    uint64_t data_used;

    m15_mcsfs1_inode_t
        inodes[M15_MCSFS1_MAX_FILES];

    uint8_t
        data[M15_MCSFS1_DATA_SIZE];

} m15_mcsfs1_t;

void m15_mcsfs1_init(
    m15_mcsfs1_t *fs
);

int m15_mcsfs1_create(
    m15_mcsfs1_t *fs,
    const char *name
);

int m15_mcsfs1_write(
    m15_mcsfs1_t *fs,
    const char *name,
    const void *buffer,
    uint64_t size
);

int64_t m15_mcsfs1_read(
    m15_mcsfs1_t *fs,
    const char *name,
    void *buffer,
    uint64_t size
);

#endif

#ifndef MCSOS_VFS_MCS_VFS_H
#define MCSOS_VFS_MCS_VFS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MCS_MAX_NAME 32u
#define MCS_MAX_PATH 128u
#define MCS_MAX_NODES 64u
#define MCS_MAX_OPEN_FILES 16u
#define MCS_RAMFS_DATA_BYTES 8192u

#define MCS_O_RDONLY 0x0001u
#define MCS_O_WRONLY 0x0002u
#define MCS_O_RDWR   0x0004u
#define MCS_O_CREAT  0x0100u
#define MCS_O_TRUNC  0x0200u
#define MCS_O_APPEND 0x0400u

#define MCS_SEEK_SET 0
#define MCS_SEEK_CUR 1
#define MCS_SEEK_END 2

#define MCS_OK        0
#define MCS_EINVAL   -1
#define MCS_ENOENT   -2
#define MCS_ENFILE   -3
#define MCS_EBADF    -4
#define MCS_ENOSPC   -5

typedef enum {
    MCS_VNODE_DIR = 1,
    MCS_VNODE_FILE = 2
} mcs_vnode_type_t;

typedef struct {
    uint32_t id;
    uint32_t parent;
    mcs_vnode_type_t type;

    char name[MCS_MAX_NAME];

    uint64_t size;

    uint64_t data_offset;
    uint64_t data_capacity;
} mcs_vnode_t;

typedef struct {
    mcs_vnode_t nodes[MCS_MAX_NODES];

    uint8_t data[MCS_RAMFS_DATA_BYTES];

    uint32_t node_count;

    uint64_t data_used;
} mcs_ramfs_t;

typedef struct {
    uint32_t flags;

    uint64_t offset;

    mcs_vnode_t *node;

    mcs_ramfs_t *fs;

    bool used;
} mcs_file_t;

typedef struct {
    mcs_file_t files[MCS_MAX_OPEN_FILES];
} mcs_fd_table_t;

void mcs_ramfs_init(
    mcs_ramfs_t *fs
);

int mcs_ramfs_seed_file(
    mcs_ramfs_t *fs,
    const char *path,
    const uint8_t *data,
    uint64_t size
);

mcs_vnode_t *mcs_ramfs_lookup(
    mcs_ramfs_t *fs,
    const char *path
);

mcs_vnode_t *mcs_ramfs_create_file(
    mcs_ramfs_t *fs,
    const char *path
);

void mcs_fd_table_init(
    mcs_fd_table_t *table
);

int mcs_vfs_open(
    mcs_fd_table_t *table,
    mcs_ramfs_t *fs,
    const char *path,
    uint32_t flags
);

int64_t mcs_vfs_read(
    mcs_fd_table_t *table,
    int fd,
    void *buffer,
    uint64_t size
);

int64_t mcs_vfs_write(
    mcs_fd_table_t *table,
    int fd,
    const void *buffer,
    uint64_t size
);

int64_t mcs_vfs_lseek(
    mcs_fd_table_t *table,
    int fd,
    int64_t offset,
    int whence
);

int mcs_vfs_close(
    mcs_fd_table_t *table,
    int fd
);

extern mcs_ramfs_t
    *mcs_active_ramfs_for_test;

void mcs_vfs_set_active_ramfs_for_test(
    mcs_ramfs_t *fs
);

#endif

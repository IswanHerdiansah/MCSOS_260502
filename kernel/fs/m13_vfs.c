#include <mcsos/vfs/mcs_vfs.h>

#include <stddef.h>
#include <stdint.h>

mcs_ramfs_t
    *mcs_active_ramfs_for_test =
        (mcs_ramfs_t *)0;

void mcs_vfs_set_active_ramfs_for_test(
    mcs_ramfs_t *fs
) {
    mcs_active_ramfs_for_test = fs;
}

void mcs_fd_table_init(
    mcs_fd_table_t *table
) {
    for (uint32_t i = 0;
         i < MCS_MAX_OPEN_FILES;
         ++i) {

        table->files[i].used = false;
    }
}

int mcs_vfs_open(
    mcs_fd_table_t *table,
    mcs_ramfs_t *fs,
    const char *path,
    uint32_t flags
) {
    mcs_vnode_t *node =
        mcs_ramfs_lookup(
            fs,
            path
        );

    if (node == NULL) {

        if (!(flags & MCS_O_CREAT)) {
            return MCS_ENOENT;
        }

        node =
            mcs_ramfs_create_file(
                fs,
                path
            );

        if (node == NULL) {
            return MCS_ENOSPC;
        }
    }

    for (uint32_t fd = 0;
         fd < MCS_MAX_OPEN_FILES;
         ++fd) {

        if (!table->files[fd].used) {

            table->files[fd].used = true;

            table->files[fd].flags = flags;

            table->files[fd].offset = 0;

            table->files[fd].node = node;

            table->files[fd].fs = fs;

            return (int)fd;
        }
    }

    return MCS_ENFILE;
}

int64_t mcs_vfs_read(
    mcs_fd_table_t *table,
    int fd,
    void *buffer,
    uint64_t size
) {
    if (fd < 0
        || (uint32_t)fd
           >= MCS_MAX_OPEN_FILES) {

        return MCS_EBADF;
    }

    mcs_file_t *file =
        &table->files[(uint32_t)fd];

    if (!file->used) {
        return MCS_EBADF;
    }

    uint8_t *dst =
        (uint8_t *)buffer;

    uint64_t remain =
        file->node->size
        - file->offset;

    if (size > remain) {
        size = remain;
    }

    for (uint64_t i = 0;
         i < size;
         ++i) {

        dst[i] =
            file->fs->data[
                file->node->data_offset
                + file->offset
                + i
            ];
    }

    file->offset += size;

    return (int64_t)size;
}

int64_t mcs_vfs_write(
    mcs_fd_table_t *table,
    int fd,
    const void *buffer,
    uint64_t size
) {
    if (fd < 0
        || (uint32_t)fd
           >= MCS_MAX_OPEN_FILES) {

        return MCS_EBADF;
    }

    mcs_file_t *file =
        &table->files[(uint32_t)fd];

    if (!file->used) {
        return MCS_EBADF;
    }

    const uint8_t *src =
        (const uint8_t *)buffer;

    if ((file->offset + size)
        > file->node->data_capacity) {

        return MCS_ENOSPC;
    }

    for (uint64_t i = 0;
         i < size;
         ++i) {

        file->fs->data[
            file->node->data_offset
            + file->offset
            + i
        ] = src[i];
    }

    file->offset += size;

    if (file->offset
        > file->node->size) {

        file->node->size =
            file->offset;
    }

    return (int64_t)size;
}

int64_t mcs_vfs_lseek(
    mcs_fd_table_t *table,
    int fd,
    int64_t offset,
    int whence
) {
    if (fd < 0
        || (uint32_t)fd
           >= MCS_MAX_OPEN_FILES) {

        return MCS_EBADF;
    }

    mcs_file_t *file =
        &table->files[(uint32_t)fd];

    if (!file->used) {
        return MCS_EBADF;
    }

    uint64_t new_offset = 0;

    switch (whence) {

    case MCS_SEEK_SET:
        new_offset = (uint64_t)offset;
        break;

    case MCS_SEEK_CUR:
        new_offset =
            file->offset
            + (uint64_t)offset;
        break;

    case MCS_SEEK_END:
        new_offset =
            file->node->size
            + (uint64_t)offset;
        break;

    default:
        return MCS_EINVAL;
    }

    if (new_offset
        > file->node->size) {

        return MCS_EINVAL;
    }

    file->offset = new_offset;

    return (int64_t)new_offset;
}

int mcs_vfs_close(
    mcs_fd_table_t *table,
    int fd
) {
    if (fd < 0
        || (uint32_t)fd
           >= MCS_MAX_OPEN_FILES) {

        return MCS_EBADF;
    }

    table->files[
        (uint32_t)fd
    ].used = false;

    return MCS_OK;
}

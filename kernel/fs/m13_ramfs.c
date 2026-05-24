#include <mcsos/vfs/mcs_vfs.h>

#include <stddef.h>
#include <stdint.h>

static int m13_strcmp(
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

static void m13_strcpy(
    char *dst,
    const char *src
) {
    while (*src) {
        *dst++ = *src++;
    }

    *dst = '\0';
}

void mcs_ramfs_init(
    mcs_ramfs_t *fs
) {
    fs->node_count = 1;

    fs->data_used = 0;

    fs->nodes[0].id = 0;
    fs->nodes[0].parent = 0;
    fs->nodes[0].type = MCS_VNODE_DIR;

    fs->nodes[0].size = 0;

    fs->nodes[0].data_offset = 0;

    fs->nodes[0].data_capacity = 0;

    m13_strcpy(
        fs->nodes[0].name,
        "/"
    );
}

mcs_vnode_t *mcs_ramfs_lookup(
    mcs_ramfs_t *fs,
    const char *path
) {
    for (uint32_t i = 0;
         i < fs->node_count;
         ++i) {

        if (m13_strcmp(
                fs->nodes[i].name,
                path
            ) == 0) {

            return &fs->nodes[i];
        }
    }

    return NULL;
}

mcs_vnode_t *mcs_ramfs_create_file(
    mcs_ramfs_t *fs,
    const char *path
) {
    if (fs->node_count >= MCS_MAX_NODES) {
        return NULL;
    }

    if ((fs->data_used + 256u)
        >= MCS_RAMFS_DATA_BYTES) {

        return NULL;
    }

    mcs_vnode_t *node =
        &fs->nodes[fs->node_count];

    node->id = fs->node_count;

    node->parent = 0;

    node->type = MCS_VNODE_FILE;

    node->size = 0;

    node->data_offset =
        fs->data_used;

    node->data_capacity = 256u;

    m13_strcpy(
        node->name,
        path
    );

    fs->data_used +=
        node->data_capacity;

    ++fs->node_count;

    return node;
}

int mcs_ramfs_seed_file(
    mcs_ramfs_t *fs,
    const char *path,
    const uint8_t *data,
    uint64_t size
) {
    mcs_vnode_t *node =
        mcs_ramfs_create_file(
            fs,
            path
        );

    if (node == NULL) {
        return MCS_ENOSPC;
    }

    if (size > node->data_capacity) {
        return MCS_ENOSPC;
    }

    for (uint64_t i = 0;
         i < size;
         ++i) {

        fs->data[
            node->data_offset + i
        ] = data[i];
    }

    node->size = size;

    return MCS_OK;
}

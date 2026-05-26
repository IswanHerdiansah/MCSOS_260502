#ifndef MCSOS_FS_M16_JOURNAL_H
#define MCSOS_FS_M16_JOURNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define M16_JOURNAL_MAGIC 0x4D31364Au
#define M16_JOURNAL_MAX_ENTRIES 32u
#define M16_JOURNAL_BLOCK_SIZE 512u

#define M16_JOURNAL_OK          0
#define M16_JOURNAL_EINVAL     -1
#define M16_JOURNAL_EFULL      -2
#define M16_JOURNAL_ECORRUPT   -3

typedef struct {

    uint64_t target_block;

    uint8_t data[M16_JOURNAL_BLOCK_SIZE];

    bool valid;

} m16_journal_entry_t;

typedef struct {

    uint32_t magic;

    uint32_t committed;

    uint32_t entry_count;

    m16_journal_entry_t
        entries[M16_JOURNAL_MAX_ENTRIES];

} m16_journal_t;

void m16_journal_init(
    m16_journal_t *journal
);

int m16_journal_append(
    m16_journal_t *journal,
    uint64_t target_block,
    const void *data
);

int m16_journal_commit(
    m16_journal_t *journal
);

int m16_journal_replay(
    m16_journal_t *journal,
    uint8_t *disk,
    uint64_t disk_block_count
);

int m16_journal_fsck(
    m16_journal_t *journal
);

void m16_journal_clear(
    m16_journal_t *journal
);

#endif

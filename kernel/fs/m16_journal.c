#include <mcsos/fs/m16_journal.h>

static void m16_memcpy(
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

static void m16_memzero(
    uint8_t *dst,
    uint64_t size
) {
    for (uint64_t i = 0;
         i < size;
         ++i) {

        dst[i] = 0;
    }
}

void m16_journal_init(
    m16_journal_t *journal
) {
    if (journal == 0) {
        return;
    }

    m16_memzero(
        (uint8_t *)journal,
        sizeof(*journal)
    );

    journal->magic =
        M16_JOURNAL_MAGIC;
}

int m16_journal_append(
    m16_journal_t *journal,
    uint64_t target_block,
    const void *data
) {
    if (journal == 0 || data == 0) {
        return M16_JOURNAL_EINVAL;
    }

    if (journal->entry_count >=
        M16_JOURNAL_MAX_ENTRIES) {

        return M16_JOURNAL_EFULL;
    }

    m16_journal_entry_t *entry =
        &journal->entries[
            journal->entry_count
        ];

    entry->target_block =
        target_block;

    entry->valid = true;

    m16_memcpy(
        entry->data,
        (const uint8_t *)data,
        M16_JOURNAL_BLOCK_SIZE
    );

    ++journal->entry_count;

    return M16_JOURNAL_OK;
}

int m16_journal_commit(
    m16_journal_t *journal
) {
    if (journal == 0) {
        return M16_JOURNAL_EINVAL;
    }

    journal->committed = 1;

    return M16_JOURNAL_OK;
}

int m16_journal_replay(
    m16_journal_t *journal,
    uint8_t *disk,
    uint64_t disk_block_count
) {
    if (journal == 0 || disk == 0) {
        return M16_JOURNAL_EINVAL;
    }

    if (journal->magic !=
        M16_JOURNAL_MAGIC) {

        return M16_JOURNAL_ECORRUPT;
    }

    if (journal->committed == 0) {
        return M16_JOURNAL_OK;
    }

    for (uint32_t i = 0;
         i < journal->entry_count;
         ++i) {

        m16_journal_entry_t *entry =
            &journal->entries[i];

        if (!entry->valid) {
            continue;
        }

        if (entry->target_block >=
            disk_block_count) {

            return M16_JOURNAL_ECORRUPT;
        }

        uint8_t *dst =
            &disk[
                entry->target_block *
                M16_JOURNAL_BLOCK_SIZE
            ];

        m16_memcpy(
            dst,
            entry->data,
            M16_JOURNAL_BLOCK_SIZE
        );
    }

    return M16_JOURNAL_OK;
}

int m16_journal_fsck(
    m16_journal_t *journal
) {
    if (journal == 0) {
        return M16_JOURNAL_EINVAL;
    }

    if (journal->magic !=
        M16_JOURNAL_MAGIC) {

        return M16_JOURNAL_ECORRUPT;
    }

    if (journal->entry_count >
        M16_JOURNAL_MAX_ENTRIES) {

        return M16_JOURNAL_ECORRUPT;
    }

    return M16_JOURNAL_OK;
}

void m16_journal_clear(
    m16_journal_t *journal
) {
    if (journal == 0) {
        return;
    }

    uint32_t magic =
        journal->magic;

    m16_memzero(
        (uint8_t *)journal,
        sizeof(*journal)
    );

    journal->magic = magic;
}

#include <mcsos/fs/m16_journal.h>

#include <stdint.h>
#include <stdio.h>

static int failures = 0;

static void expect(
    int cond,
    const char *msg
) {
    if (!cond) {

        ++failures;

        printf(
            "FAIL: %s\n",
            msg
        );
    }
}

int main(void) {

    m16_journal_t journal;

    uint8_t disk[
        8 * M16_JOURNAL_BLOCK_SIZE
    ];

    uint8_t block[
        M16_JOURNAL_BLOCK_SIZE
    ];

    for (uint64_t i = 0;
         i < sizeof(disk);
         ++i) {

        disk[i] = 0;
    }

    for (uint64_t i = 0;
         i < sizeof(block);
         ++i) {

        block[i] =
            (uint8_t)(i & 0xFFu);
    }

    m16_journal_init(
        &journal
    );

    expect(
        m16_journal_fsck(
            &journal
        ) == M16_JOURNAL_OK,
        "initial fsck"
    );

    expect(
        m16_journal_append(
            &journal,
            2,
            block
        ) == M16_JOURNAL_OK,
        "append entry"
    );

    expect(
        journal.entry_count == 1,
        "entry count"
    );

    expect(
        m16_journal_commit(
            &journal
        ) == M16_JOURNAL_OK,
        "commit"
    );

    expect(
        m16_journal_replay(
            &journal,
            disk,
            8
        ) == M16_JOURNAL_OK,
        "replay"
    );

    expect(
        disk[
            2 *
            M16_JOURNAL_BLOCK_SIZE
        ] == 0,
        "replay data"
    );

    expect(
        disk[
            (2 *
             M16_JOURNAL_BLOCK_SIZE)
            + 1
        ] == 1,
        "replay data byte"
    );

    m16_journal_clear(
        &journal
    );

    expect(
        journal.entry_count == 0,
        "clear entries"
    );

    journal.magic = 0;

    expect(
        m16_journal_fsck(
            &journal
        ) == M16_JOURNAL_ECORRUPT,
        "corrupt detect"
    );

    if (failures != 0) {

        printf(
            "M16 host test FAIL: %d\n",
            failures
        );

        return 1;
    }

    printf(
        "M16 host test PASS\n"
    );

    return 0;
}

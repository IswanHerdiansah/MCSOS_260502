#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <mcsos/kernel/vmm.h>

static vmm_pte_t pml4[512] __attribute__((aligned(4096)));

static vmm_pte_t tables[512 * 8]
    __attribute__((aligned(4096)));

int main(void) {

    memset(pml4, 0, sizeof(pml4));
    memset(tables, 0, sizeof(tables));

    struct vmm_space space = {
        .pml4 = pml4
    };

    bool ok;

    ok = vmm_map_page(
        &space,
        0x400000ULL,
        0x200000ULL,
        VMM_WRITABLE,
        tables,
        8
    );

    assert(ok);

    uint64_t phys = 0;
    uint64_t flags = 0;

    ok = vmm_query_page(
        &space,
        0x400000ULL,
        &phys,
        &flags
    );

    assert(ok);

    assert(phys == 0x200000ULL);

    assert((flags & VMM_PRESENT) != 0);

    ok = vmm_unmap_page(
        &space,
        0x400000ULL
    );

    assert(ok);

    ok = vmm_query_page(
        &space,
        0x400000ULL,
        &phys,
        &flags
    );

    assert(!ok);

    puts("M7 VMM host unit test: PASS");

    return 0;
}

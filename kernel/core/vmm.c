#include <mcsos/kernel/vmm.h>

#define PT_ENTRIES 512

typedef struct {
    uint64_t virt;
    uint64_t phys;
    uint64_t flags;
    bool used;
} vmm_mapping_t;

static vmm_mapping_t g_mappings[256];

bool vmm_is_canonical(uint64_t virt) {

    uint64_t upper = virt >> 48;

    return (upper == 0ULL || upper == 0xFFFFULL);
}

bool vmm_map_page(
    struct vmm_space *space,
    uint64_t virt,
    uint64_t phys,
    uint64_t flags,
    vmm_pte_t *tables,
    size_t table_count
) {

    (void)space;
    (void)tables;
    (void)table_count;

    if (!vmm_is_canonical(virt))
        return false;

    if (virt & 0xFFFULL)
        return false;

    if (phys & 0xFFFULL)
        return false;

    for (size_t i = 0; i < 256; i++) {

        if (!g_mappings[i].used) {

            g_mappings[i].virt = virt;
            g_mappings[i].phys = phys;
            g_mappings[i].flags =
                flags | VMM_PRESENT;

            g_mappings[i].used = true;

            return true;
        }
    }

    return false;
}

bool vmm_query_page(
    struct vmm_space *space,
    uint64_t virt,
    uint64_t *phys_out,
    uint64_t *flags_out
) {

    (void)space;

    for (size_t i = 0; i < 256; i++) {

        if (!g_mappings[i].used)
            continue;

        if (g_mappings[i].virt != virt)
            continue;

        if (phys_out)
            *phys_out = g_mappings[i].phys;

        if (flags_out)
            *flags_out = g_mappings[i].flags;

        return true;
    }

    return false;
}

bool vmm_unmap_page(
    struct vmm_space *space,
    uint64_t virt
) {

    (void)space;

    for (size_t i = 0; i < 256; i++) {

        if (!g_mappings[i].used)
            continue;

        if (g_mappings[i].virt != virt)
            continue;

        g_mappings[i].used = false;

        return true;
    }

    return false;
}

#ifndef MCSOS_HOST_TEST

uint64_t vmm_read_cr2(void) {

    uint64_t value;

    __asm__ volatile (
        "mov %%cr2, %0"
        : "=r"(value)
    );

    return value;
}

uint64_t vmm_read_cr3(void) {

    uint64_t value;

    __asm__ volatile (
        "mov %%cr3, %0"
        : "=r"(value)
    );

    return value;
}

void vmm_invlpg(void *addr) {

    __asm__ volatile (
        "invlpg (%0)"
        :
        : "r"(addr)
        : "memory"
    );
}

#else

uint64_t vmm_read_cr2(void) {
    return 0;
}

uint64_t vmm_read_cr3(void) {
    return 0;
}

void vmm_invlpg(void *addr) {
    (void)addr;
}

#endif

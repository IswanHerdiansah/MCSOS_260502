#ifndef MCSOS_KERNEL_VMM_H
#define MCSOS_KERNEL_VMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VMM_PAGE_SIZE 4096ULL

#define VMM_PRESENT  (1ULL << 0)
#define VMM_WRITABLE (1ULL << 1)
#define VMM_USER     (1ULL << 2)
#define VMM_HUGE     (1ULL << 7)
#define VMM_NOEXEC   (1ULL << 63)

typedef uint64_t vmm_pte_t;

struct vmm_space {
    vmm_pte_t *pml4;
};

bool vmm_is_canonical(uint64_t virt);

bool vmm_map_page(
    struct vmm_space *space,
    uint64_t virt,
    uint64_t phys,
    uint64_t flags,
    vmm_pte_t *tables,
    size_t table_count
);

bool vmm_unmap_page(
    struct vmm_space *space,
    uint64_t virt
);

bool vmm_query_page(
    struct vmm_space *space,
    uint64_t virt,
    uint64_t *phys_out,
    uint64_t *flags_out
);

uint64_t vmm_read_cr2(void);
uint64_t vmm_read_cr3(void);

void vmm_invlpg(void *addr);

#endif

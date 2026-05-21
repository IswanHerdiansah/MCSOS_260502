#ifndef MCSOS_KERNEL_PMM_H
#define MCSOS_KERNEL_PMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PMM_PAGE_SIZE 4096ULL
#define PMM_INVALID_FRAME 0xffffffffffffffffULL

#define PMM_REGION_USABLE 1
#define PMM_REGION_RESERVED 2

#define PMM_MAX_PHYS_BYTES (64ULL * 1024ULL * 1024ULL)
#define PMM_BITMAP_BYTES ((PMM_MAX_PHYS_BYTES / PMM_PAGE_SIZE) / 8ULL)

struct boot_mem_region {
    uint64_t base;
    uint64_t length;
    uint32_t type;
};

struct pmm_state {
    uint8_t *bitmap;
    uint64_t frame_count;
    uint64_t free_frames;
    uint64_t used_frames;
    uint64_t next_hint;
    bool initialized;
};

void pmm_zero_state(struct pmm_state *pmm);

bool pmm_init_from_map(
    struct pmm_state *pmm,
    const struct boot_mem_region *regions,
    size_t region_count,
    uint8_t *bitmap_storage,
    size_t bitmap_size,
    uint64_t max_phys_bytes
);

uint64_t pmm_alloc_frame(struct pmm_state *pmm);

bool pmm_free_frame(struct pmm_state *pmm, uint64_t phys);

bool pmm_reserve_range(
    struct pmm_state *pmm,
    uint64_t base,
    uint64_t length
);

bool pmm_is_frame_free(
    const struct pmm_state *pmm,
    uint64_t phys
);

uint64_t pmm_free_count(const struct pmm_state *pmm);
uint64_t pmm_used_count(const struct pmm_state *pmm);
uint64_t pmm_frame_count(const struct pmm_state *pmm);

#endif

#include <assert.h>
#include <stdio.h>

#include <mcsos/kernel/pmm.h>

int main(void) {

    static uint8_t bitmap[PMM_BITMAP_BYTES];

    struct boot_mem_region regions[] = {
        {0x00000000ULL, 0x0009F000ULL, PMM_REGION_RESERVED},
        {0x00100000ULL, 0x00300000ULL, PMM_REGION_USABLE},
        {0x00400000ULL, 0x00100000ULL, PMM_REGION_RESERVED},
    };

    struct pmm_state pmm;

    assert(pmm_init_from_map(
        &pmm,
        regions,
        3,
        bitmap,
        sizeof(bitmap),
        PMM_MAX_PHYS_BYTES
    ));

    uint64_t frame = pmm_alloc_frame(&pmm);

    assert(frame != PMM_INVALID_FRAME);

    assert(pmm_free_frame(&pmm, frame));

    puts("M6 PMM host unit test: PASS");

    return 0;
}

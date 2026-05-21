#include <mcsos/kernel/pmm.h>

static void bitmap_set(uint8_t *bitmap, uint64_t bit) {
    bitmap[bit / 8ULL] |= (1u << (bit % 8ULL));
}

static void bitmap_clear(uint8_t *bitmap, uint64_t bit) {
    bitmap[bit / 8ULL] &= ~(1u << (bit % 8ULL));
}

static bool bitmap_test(const uint8_t *bitmap, uint64_t bit) {
    return (bitmap[bit / 8ULL] & (1u << (bit % 8ULL))) != 0;
}

void pmm_zero_state(struct pmm_state *pmm) {
    pmm->bitmap = 0;
    pmm->frame_count = 0;
    pmm->free_frames = 0;
    pmm->used_frames = 0;
    pmm->next_hint = 0;
    pmm->initialized = false;
}

bool pmm_init_from_map(
    struct pmm_state *pmm,
    const struct boot_mem_region *regions,
    size_t region_count,
    uint8_t *bitmap_storage,
    size_t bitmap_size,
    uint64_t max_phys_bytes
) {
    (void)bitmap_size;

    pmm_zero_state(pmm);

    pmm->bitmap = bitmap_storage;
    pmm->frame_count = max_phys_bytes / PMM_PAGE_SIZE;

    for (uint64_t i = 0; i < pmm->frame_count / 8ULL; i++) {
        pmm->bitmap[i] = 0xFF;
    }

    pmm->used_frames = pmm->frame_count;
    pmm->free_frames = 0;

    for (size_t r = 0; r < region_count; r++) {

        if (regions[r].type != PMM_REGION_USABLE)
            continue;

        uint64_t start = regions[r].base / PMM_PAGE_SIZE;
        uint64_t count = regions[r].length / PMM_PAGE_SIZE;

        for (uint64_t i = 0; i < count; i++) {

            uint64_t frame = start + i;

            if (frame >= pmm->frame_count)
                break;

            if (bitmap_test(pmm->bitmap, frame)) {
                bitmap_clear(pmm->bitmap, frame);
                pmm->free_frames++;
                pmm->used_frames--;
            }
        }
    }

    bitmap_set(pmm->bitmap, 0);

    pmm->free_frames--;
    pmm->used_frames++;

    pmm->initialized = true;

    return true;
}

uint64_t pmm_alloc_frame(struct pmm_state *pmm) {

    for (uint64_t i = 0; i < pmm->frame_count; i++) {

        if (!bitmap_test(pmm->bitmap, i)) {

            bitmap_set(pmm->bitmap, i);

            pmm->free_frames--;
            pmm->used_frames++;

            return i * PMM_PAGE_SIZE;
        }
    }

    return PMM_INVALID_FRAME;
}

bool pmm_free_frame(struct pmm_state *pmm, uint64_t phys) {

    if ((phys & (PMM_PAGE_SIZE - 1ULL)) != 0)
        return false;

    uint64_t frame = phys / PMM_PAGE_SIZE;

    if (frame >= pmm->frame_count)
        return false;

    if (!bitmap_test(pmm->bitmap, frame))
        return false;

    bitmap_clear(pmm->bitmap, frame);

    pmm->free_frames++;
    pmm->used_frames--;

    return true;
}

bool pmm_reserve_range(
    struct pmm_state *pmm,
    uint64_t base,
    uint64_t length
) {

    uint64_t start = base / PMM_PAGE_SIZE;
    uint64_t count = length / PMM_PAGE_SIZE;

    for (uint64_t i = 0; i < count; i++) {

        uint64_t frame = start + i;

        if (frame >= pmm->frame_count)
            break;

        if (!bitmap_test(pmm->bitmap, frame)) {

            bitmap_set(pmm->bitmap, frame);

            pmm->free_frames--;
            pmm->used_frames++;
        }
    }

    return true;
}

bool pmm_is_frame_free(
    const struct pmm_state *pmm,
    uint64_t phys
) {

    uint64_t frame = phys / PMM_PAGE_SIZE;

    if (frame >= pmm->frame_count)
        return false;

    return !bitmap_test(pmm->bitmap, frame);
}

uint64_t pmm_free_count(const struct pmm_state *pmm) {
    return pmm->free_frames;
}

uint64_t pmm_used_count(const struct pmm_state *pmm) {
    return pmm->used_frames;
}

uint64_t pmm_frame_count(const struct pmm_state *pmm) {
    return pmm->frame_count;
}

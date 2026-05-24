#include <mcsos/user/m11_elf_loader.h>

enum m11_status m11_elf64_plan_load(
    const void *image,
    uint64_t image_size,
    struct m11_process_image_plan *plan
) {
    const uint8_t *bytes;

    const struct m11_elf64_ehdr *ehdr;

    const struct m11_elf64_phdr *phdr;

    uint16_t i;

    if (image == 0 || plan == 0) {
        return M11_ERR_MAGIC;
    }

    if (image_size <
        sizeof(struct m11_elf64_ehdr)) {
        return M11_ERR_MAGIC;
    }

    bytes = (const uint8_t *) image;

    ehdr =
        (const struct m11_elf64_ehdr *)
        bytes;

    if (ehdr->e_ident_magic !=
        M11_ELF_MAGIC) {
        return M11_ERR_MAGIC;
    }

    if (ehdr->e_machine !=
        M11_ELF_MACHINE_X86_64) {
        return M11_ERR_MACHINE;
    }

    if (ehdr->e_entry <
        0x400000ULL) {
        return M11_ERR_ENTRY;
    }

    if ((ehdr->e_phoff +
        ((uint64_t) ehdr->e_phnum *
        sizeof(struct m11_elf64_phdr)))
        > image_size) {
        return M11_ERR_SEGBOUNDS;
    }

    phdr =
        (const struct m11_elf64_phdr *)
        (bytes + ehdr->e_phoff);

    plan->entry_virtual_address =
        ehdr->e_entry;

    plan->segment_count = 0;

    for (i = 0; i < ehdr->e_phnum; ++i) {

        const struct m11_elf64_phdr *seg;

        seg = &phdr[i];

        if (seg->p_type !=
            M11_PT_LOAD) {
            continue;
        }

        if (seg->p_memsz <
            seg->p_filesz) {
            return M11_ERR_SEGBOUNDS;
        }

        if ((seg->p_offset +
            seg->p_filesz)
            > image_size) {
            return M11_ERR_SEGBOUNDS;
        }

        if (seg->p_align == 0) {
            return M11_ERR_ALIGN;
        }

        if ((seg->p_align &
            (seg->p_align - 1)) != 0) {
            return M11_ERR_ALIGN;
        }

        if (seg->p_vaddr <
            0x400000ULL) {
            return M11_ERR_SEGRANGE;
        }

        ++plan->segment_count;
    }

    return M11_OK;
}

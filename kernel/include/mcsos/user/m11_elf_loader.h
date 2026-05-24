#ifndef MCSOS_USER_M11_ELF_LOADER_H
#define MCSOS_USER_M11_ELF_LOADER_H

#include <stdint.h>

#define M11_ELF_MAGIC 0x464c457fU
#define M11_ELF_MACHINE_X86_64 62
#define M11_PT_LOAD 1

enum m11_status {
    M11_OK = 0,
    M11_ERR_MAGIC,
    M11_ERR_MACHINE,
    M11_ERR_ENTRY,
    M11_ERR_SEGBOUNDS,
    M11_ERR_ALIGN,
    M11_ERR_SEGRANGE
};

struct __attribute__((packed))
m11_elf64_ehdr {
    uint32_t e_ident_magic;

    uint16_t e_machine;

    uint64_t e_entry;

    uint64_t e_phoff;

    uint16_t e_phnum;
};

struct __attribute__((packed))
m11_elf64_phdr {
    uint32_t p_type;

    uint64_t p_offset;

    uint64_t p_vaddr;

    uint64_t p_filesz;

    uint64_t p_memsz;

    uint64_t p_align;
};

struct m11_process_image_plan {
    uint64_t entry_virtual_address;

    uint32_t segment_count;
};

enum m11_status m11_elf64_plan_load(
    const void *image,
    uint64_t image_size,
    struct m11_process_image_plan *plan
);

#endif

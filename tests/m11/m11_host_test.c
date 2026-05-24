#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <mcsos/user/m11_elf_loader.h>

static void build_valid_elf(
    uint8_t *buffer
) {
    struct m11_elf64_ehdr *ehdr;

    struct m11_elf64_phdr *phdr;

    memset(buffer, 0, 16384);

    ehdr =
        (struct m11_elf64_ehdr *)
        buffer;

    ehdr->e_ident_magic =
        M11_ELF_MAGIC;

    ehdr->e_machine =
        M11_ELF_MACHINE_X86_64;

    ehdr->e_entry =
        0x401000ULL;

    ehdr->e_phoff =
        24;

    ehdr->e_phnum =
        2;

    phdr =
        (struct m11_elf64_phdr *)
        (buffer + 24);

    phdr[0].p_type =
        M11_PT_LOAD;

    phdr[0].p_offset =
        0x100;

    phdr[0].p_vaddr =
        0x401000ULL;

    phdr[0].p_filesz =
        0x1000;

    phdr[0].p_memsz =
        0x2000;

    phdr[0].p_align =
        0x1000;

    phdr[1].p_type =
        M11_PT_LOAD;

    phdr[1].p_offset =
        0x2000;

    phdr[1].p_vaddr =
        0x404000ULL;

    phdr[1].p_filesz =
        0x1000;

    phdr[1].p_memsz =
        0x2000;

    phdr[1].p_align =
        0x1000;
}

int main(void) {
    uint8_t image[16384];

    struct m11_process_image_plan
        plan;

    enum m11_status rc;

    build_valid_elf(image);

    rc = m11_elf64_plan_load(
        image,
        sizeof(image),
        &plan
    );

    assert(rc == M11_OK);

    printf(
        "PASS valid ELF64 image: "
        "M11_OK\n"
    );

    printf(
        "PASS valid plan fields: "
        "entry=0x%llx segments=%u\n",
        (unsigned long long)
        plan.entry_virtual_address,
        plan.segment_count
    );

    image[0] = 0;

    rc = m11_elf64_plan_load(
        image,
        sizeof(image),
        &plan
    );

    assert(rc == M11_ERR_MAGIC);

    printf(
        "PASS bad magic: "
        "M11_ERR_MAGIC\n"
    );

    build_valid_elf(image);

    ((struct m11_elf64_ehdr *)
        image)->e_machine = 0;

    rc = m11_elf64_plan_load(
        image,
        sizeof(image),
        &plan
    );

    assert(rc ==
        M11_ERR_MACHINE);

    printf(
        "PASS bad machine: "
        "M11_ERR_MACHINE\n"
    );

    build_valid_elf(image);

    ((struct m11_elf64_ehdr *)
        image)->e_entry = 0;

    rc = m11_elf64_plan_load(
        image,
        sizeof(image),
        &plan
    );

    assert(rc ==
        M11_ERR_ENTRY);

    printf(
        "PASS entry outside "
        "user range: "
        "M11_ERR_ENTRY\n"
    );

    build_valid_elf(image);

    ((struct m11_elf64_phdr *)
        (image + 24))
            ->p_memsz = 1;

    rc = m11_elf64_plan_load(
        image,
        sizeof(image),
        &plan
    );

    assert(rc ==
        M11_ERR_SEGBOUNDS);

    printf(
        "PASS memsz below "
        "filesz: "
        "M11_ERR_SEGBOUNDS\n"
    );

    puts(
        "M11 host tests passed."
    );

    return 0;
}

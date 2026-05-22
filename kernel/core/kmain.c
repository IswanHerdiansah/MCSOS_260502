#include <stdint.h>

#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>
#include <mcsos/arch/pic.h>
#include <mcsos/arch/pit.h>

#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/kernel/version.h>

#include <mcsos/kernel/pmm.h>
#include <mcsos/kernel/vmm.h>
#include <mcsos/kernel/kmem.h>

extern char __kernel_start[];
extern char __kernel_end[];

static struct pmm_state kernel_pmm;

static uint8_t kernel_pmm_bitmap[PMM_BITMAP_BYTES];

static struct boot_mem_region m6_regions[] = {
    {0x00000000ULL, 0x0009F000ULL, PMM_REGION_RESERVED},
    {0x00100000ULL, 0x00300000ULL, PMM_REGION_USABLE},
};

static vmm_pte_t kernel_pml4[512];

static vmm_pte_t kernel_tables[512 * 8];

#define M8_BOOT_HEAP_SIZE (64u * 1024u)

static uint8_t m8_boot_heap[M8_BOOT_HEAP_SIZE]
    __attribute__((aligned(4096)));

static void m5_selftest(void) {

    KERNEL_ASSERT(__kernel_end > __kernel_start);

    KERNEL_ASSERT(sizeof(uintptr_t) == 8u);

    KERNEL_ASSERT(sizeof(x86_64_idt_entry_t) == 16u);

    KERNEL_ASSERT(x86_64_idt_base_for_test() != 0u);

    KERNEL_ASSERT(x86_64_idt_limit_for_test() == 4095u);

    log_writeln("[M5] selftest: IDT invariants passed");
}

static void m8_heap_bootstrap(void) {

    kmem_init(
        m8_boot_heap,
        sizeof(m8_boot_heap)
    );

    void *probe = kmalloc(128);

    KERNEL_ASSERT(probe != 0);

    kfree(probe);

    log_writeln("[M8] kmem initialized");
}

void kmain(void) {

    cpu_cli();

    log_init();

    log_write(MCSOS_NAME);
    log_write(" ");
    log_write(MCSOS_VERSION);
    log_write(" ");
    log_write(MCSOS_MILESTONE);
    log_writeln(" [M8] kernel heap allocator");

    log_key_value_hex64(
        "kernel_start",
        (uint64_t)(uintptr_t)__kernel_start
    );

    log_key_value_hex64(
        "kernel_end",
        (uint64_t)(uintptr_t)__kernel_end
    );

    bool pmm_ok = pmm_init_from_map(
        &kernel_pmm,
        m6_regions,
        2,
        kernel_pmm_bitmap,
        sizeof(kernel_pmm_bitmap),
        PMM_MAX_PHYS_BYTES
    );

    KERNEL_ASSERT(pmm_ok);

    log_writeln("[M6] pmm initialized");

    struct vmm_space kernel_space = {
        .pml4 = kernel_pml4
    };

    bool map_ok = vmm_map_page(
        &kernel_space,
        0x400000ULL,
        0x200000ULL,
        VMM_WRITABLE,
        kernel_tables,
        8
    );

    KERNEL_ASSERT(map_ok);

    log_writeln("[M7] vmm map ok");

    uint64_t phys = 0;

    uint64_t flags = 0;

    bool query_ok = vmm_query_page(
        &kernel_space,
        0x400000ULL,
        &phys,
        &flags
    );

    KERNEL_ASSERT(query_ok);

    log_write("[M7] vmm phys=");
    log_hex64(phys);
    log_writeln("");

    log_write("[M7] cr3=");
    log_hex64(vmm_read_cr3());
    log_writeln("");

    m8_heap_bootstrap();

    x86_64_idt_init();

    log_writeln("[M5] idt: loaded");

    m5_selftest();

    pic_remap(
        PIC_MASTER_OFFSET,
        PIC_SLAVE_OFFSET
    );

    pic_mask_all();

    pic_unmask_irq(0u);

    pit_configure_hz(100u);

    log_writeln("[M5] sti: enabling interrupts");

    cpu_sti();

    for (;;) {
        cpu_hlt();
    }
}

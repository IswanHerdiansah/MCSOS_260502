#include <stdint.h>
#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>
#include <mcsos/arch/pic.h>
#include <mcsos/arch/pit.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/kernel/version.h>
#include <mcsos/kernel/pmm.h>

extern char __kernel_start[];
extern char __kernel_end[];

static struct pmm_state kernel_pmm;

static uint8_t kernel_pmm_bitmap[PMM_BITMAP_BYTES];

static struct boot_mem_region m6_regions[] = {
    {0x00000000ULL, 0x0009F000ULL, PMM_REGION_RESERVED},
    {0x00100000ULL, 0x00300000ULL, PMM_REGION_USABLE},
};

static void m5_selftest(void) {
    KERNEL_ASSERT(__kernel_end > __kernel_start);
    KERNEL_ASSERT(sizeof(uintptr_t) == 8u);
    KERNEL_ASSERT(sizeof(x86_64_idt_entry_t) == 16u);
    KERNEL_ASSERT(x86_64_idt_base_for_test() != 0u);
    KERNEL_ASSERT(x86_64_idt_limit_for_test() == 4095u);
    log_writeln("[M5] selftest: IDT invariants passed");
}

void kmain(void) {
    cpu_cli();

    log_init();

    log_write(MCSOS_NAME);
    log_write(" ");
    log_write(MCSOS_VERSION);
    log_write(" ");
    log_write(MCSOS_MILESTONE);
    log_writeln(" [M5] boot: external interrupt bring-up start");

    log_key_value_hex64("kernel_start",
        (uint64_t)(uintptr_t)__kernel_start);

    log_key_value_hex64("kernel_end",
        (uint64_t)(uintptr_t)__kernel_end);

    log_writeln("[M6] pmm: init start");

    bool pmm_ok = pmm_init_from_map(
        &kernel_pmm,
        m6_regions,
        2,
        kernel_pmm_bitmap,
        sizeof(kernel_pmm_bitmap),
        PMM_MAX_PHYS_BYTES
    );

    KERNEL_ASSERT(pmm_ok);

    log_write("[M6] pmm: free_frames=");
    log_hex64(pmm_free_count(&kernel_pmm));
    log_writeln("");

    uint64_t frame = pmm_alloc_frame(&kernel_pmm);

    KERNEL_ASSERT(frame != PMM_INVALID_FRAME);

    log_write("[M6] pmm: allocated frame=");
    log_hex64(frame);
    log_writeln("");

    KERNEL_ASSERT(pmm_free_frame(&kernel_pmm, frame));

    log_writeln("[M6] pmm: alloc/free selftest passed");

    x86_64_idt_init();
    log_writeln("[M5] idt: loaded");

    m5_selftest();

    pic_remap(PIC_MASTER_OFFSET, PIC_SLAVE_OFFSET);

    pic_mask_all();

    pic_unmask_irq(0u);

    log_write("[M5] pic: remapped; master_mask=");
    log_hex64((uint64_t)pic_read_master_mask());

    log_write(" slave_mask=");
    log_hex64((uint64_t)pic_read_slave_mask());

    log_writeln("");

    pit_configure_hz(100u);

    log_writeln("[M5] pit: configured 100Hz");

    log_writeln("[M5] sti: enabling interrupts");

    cpu_sti();

#ifdef MCSOS_M4_TRIGGER_BREAKPOINT
    log_writeln("[M5] triggering breakpoint test");
    x86_64_trigger_breakpoint_for_test();
    log_writeln("[M5] returned from breakpoint");
#endif

    for (;;) {
        cpu_hlt();
    }
}

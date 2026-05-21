#include <stdint.h>
#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>
#include <mcsos/arch/pic.h>
#include <mcsos/arch/pit.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/kernel/version.h>

extern char __kernel_start[];
extern char __kernel_end[];

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
    log_write(MCSOS_NAME); log_write(" ");
    log_write(MCSOS_VERSION); log_write(" ");
    log_write(MCSOS_MILESTONE);
    log_writeln(" [M5] boot: external interrupt bring-up start");
    log_key_value_hex64("kernel_start", (uint64_t)(uintptr_t)__kernel_start);
    log_key_value_hex64("kernel_end",   (uint64_t)(uintptr_t)__kernel_end);

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

    for (;;) { cpu_hlt(); }
}

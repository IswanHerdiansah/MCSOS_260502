#include <stdint.h>
#include <mcsos/arch/idt.h>
#include <mcsos/arch/pic.h>
#include <mcsos/arch/pit.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>

static const char *exception_names[32] = {
    "#DE Divide Error", "#DB Debug", "NMI Interrupt", "#BP Breakpoint",
    "#OF Overflow", "#BR Bound Range Exceeded", "#UD Invalid Opcode",
    "#NM Device Not Available", "#DF Double Fault",
    "Coprocessor Segment Overrun", "#TS Invalid TSS",
    "#NP Segment Not Present", "#SS Stack Segment Fault",
    "#GP General Protection Fault", "#PF Page Fault", "Reserved",
    "#MF x87 Floating-Point Exception", "#AC Alignment Check",
    "#MC Machine Check", "#XM SIMD Floating-Point Exception",
    "#VE Virtualization Exception", "#CP Control Protection Exception",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "#HV Hypervisor Injection Exception", "#VC VMM Communication Exception",
    "#SX Security Exception", "Reserved"
};

static volatile uint64_t trap_count;

static const char *trap_name(uint64_t vector) {
    if (vector < 32u) return exception_names[vector];
    return "external-irq";
}

uint64_t m4_trap_count_for_test(void) { return trap_count; }

static void log_trap_frame(const x86_64_trap_frame_t *frame) {
    log_key_value_hex64("trap_vector", frame->vector);
    log_key_value_hex64("trap_error",  frame->error_code);
    log_key_value_hex64("trap_rip",    frame->rip);
    log_key_value_hex64("trap_cs",     frame->cs);
    log_key_value_hex64("trap_rflags", frame->rflags);
}

void x86_64_trap_dispatch(x86_64_trap_frame_t *frame) {
    KERNEL_ASSERT(frame != (x86_64_trap_frame_t *)0);
    ++trap_count;

    /* --- IRQ path (vector 0x20..0x2F) --- */
    if (frame->vector >= PIC_MASTER_OFFSET &&
        frame->vector <  (PIC_SLAVE_OFFSET + 8u)) {
        uint8_t irq = (uint8_t)(frame->vector - PIC_MASTER_OFFSET);
        if (irq == 0u) {
            timer_on_irq0();
        } else {
            log_write("[M5] unexpected irq=");
            log_dec64((uint64_t)irq);
            log_writeln("");
        }
        pic_send_eoi(irq);
        return;
    }

    /* --- Exception path --- */
    log_write("[M5] trap: ");
    log_writeln(trap_name(frame->vector));
    log_trap_frame(frame);

    if (frame->vector == 3u) {
        log_writeln("[M5] breakpoint handled; returning");
        return;
    }

    KERNEL_PANIC("unrecoverable CPU exception", frame->vector);
}

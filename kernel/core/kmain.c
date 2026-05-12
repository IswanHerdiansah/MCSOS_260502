#include <stdint.h>

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

static void serial_write_char(char c) {
    outb(0x3F8, (uint8_t)c);
}

static void serial_write(const char *s) {
    while (*s) {
        serial_write_char(*s++);
    }
}

void kmain(void) {
    serial_init();

    serial_write("MCSOS 260502 M2 boot path entered\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

static int serial_ready(void) {
    unsigned char r;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(r)
        : "Nd"((uint16_t)(0x3F8 + 5))
    );
    return r & 0x20;
}

static void serial_putchar(char c) {
    while (!serial_ready()) {
    }

    outb(0x3F8, (uint8_t)c);
}

void serial_write(const char *s) {
    while (*s) {
        serial_putchar(*s++);
    }
}

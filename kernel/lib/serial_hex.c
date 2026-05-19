#include <stdint.h>

extern void serial_write(const char *s);

static const char *hex = "0123456789ABCDEF";

void serial_write_hex64(uint64_t v) {
    char buf[19];
    buf[0] = '0';
    buf[1] = 'x';

    for (int i = 0; i < 16; i++) {
        buf[2 + i] = hex[(v >> (60 - i * 4)) & 0xF];
    }

    buf[18] = '\0';
    serial_write(buf);
}

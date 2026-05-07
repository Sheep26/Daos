#include "io.h"

uint8_t ps2_keyboard_input() {
	while (!(inb(0x64) & 1));

    return inb(0x60);
}

void serial_init() {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
}

void serial_print(const char *str) {
    while (!(inb(COM1 + 5) & 0x20));

    while (*str)
        outb(COM1, *str++);
}
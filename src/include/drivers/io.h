#ifndef IO_H
#define IO_H

#include <stdint.h>

#define COM1 0x3F8

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %b0, %w1"
        : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %w1, %b0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory");
    return ret;
}

static inline void insw(uint16_t port, void *addr, uint32_t count) {
    asm volatile ("rep insw"
        : "+D"(addr), "+c"(count)
        : "d"(port)
        : "memory");
}

static inline void outsw(uint16_t port, const void *addr, uint32_t count) {
    asm volatile ("rep outsw"
        : "+S"(addr), "+c"(count)
        : "d"(port)
        : "memory");
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ( "inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ( "outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

void serial_init(void);
void serial_print(const char *str);
void serial_println(const char *str);
uint8_t ps2_keyboard_input(void);

#endif
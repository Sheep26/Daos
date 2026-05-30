#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define KERNEL_CS 0x10

typedef struct {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

extern idt_entry_t idt[256];
extern idt_ptr_t idt_ptr;

void set_idt_gate(int num, uint32_t base);
void init_idt();

#endif
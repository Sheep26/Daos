#include <idt.h>
#include <string.h>
#include <itoa.h>
#include <drivers/io.h>

extern void idt_load(idt_ptr_t* ptr);

idt_entry_t idt[256] __attribute__((aligned(16)));
idt_ptr_t idt_ptr;

void set_idt_gate(int num, uint32_t base) {
    idt[num].base_lo = base;
    idt[num].base_hi = base >> 16;

    idt[num].sel = KERNEL_CS;
    idt[num].flags = 0x8E;
    idt[num].zero = 0;
}

void init_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (uint32_t) idt;

    char buf[32];
    itoa(idt_ptr.base, buf, 10);
    serial_print("IDT PTR: ");
    serial_println(buf);

    memset(idt, 0, sizeof(idt_entry_t) * 256);
    idt_load(&idt_ptr);
}
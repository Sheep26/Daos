#include <irq.h>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void *irq_routines[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void (*irq_name[32])() = {
    irq0, irq1, irq2, irq3,irq4, irq5,
    irq6, irq7, irq8, irq9, irq10,
    irq11, irq12, irq13, irq14, irq15
};

void set_irq_handler(int i, void (*handler)(reg_t *r)) {
    irq_routines[i] = handler;
}

void init_irq() {
    for (int i = 0; i < 16; i++)
        set_idt_gate(i + 32, (uint32_t) irq_name[i]);
}

void irq_handler(reg_t *r) {
    void (*handler)(struct reg *a);
    int i = r->isr_no - 32;

    handler = irq_routines[i];

    if(handler)
        handler(r);

    // EOI (End of Interrupt)
    if(i >= 8)
        outb(PIC1, 0x20);

    outb(PIC1, 0x20);
}
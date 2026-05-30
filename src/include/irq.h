#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>
#include <idt.h>
#include <pic.h>
#include <drivers/io.h>
#include <reg.h>

void set_irq_handler(int i, void (*handler) (reg_t *r));
void init_irq();
void irq_handler(reg_t *r);

#endif
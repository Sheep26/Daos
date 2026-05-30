#ifndef ISR_H
#define ISR_H

#include <stdint.h>
#include <reg.h>

void init_isr();
void isr_handler(reg_t *r);

#endif
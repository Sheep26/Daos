#ifndef APIC_H
#define APIC_H

#include <stdint.h>

#define LAPIC_BASE 0xFEE00000

#define LAPIC_REG(offset) (*(volatile uint32_t *)(LAPIC_BASE + (offset)))

#define LAPIC_ID      0x020
#define LAPIC_EOI     0x0B0
#define LAPIC_SVR     0x0F0

#define LAPIC_LVT_TMR 0x320
#define LAPIC_TMRINIT 0x380
#define LAPIC_TMRCURR 0x390
#define LAPIC_DIV     0x3E0

#define APIC_TIMER_VECTOR 0x40

extern uint64_t apic_ticks;
extern uint64_t apic_ms_passed;
extern uint64_t apic_ticks_per_ms;

void apic_enable();
void apic_timer_handler();

#endif
#include <apic.h>
#include <thread.h>
#include <pit.h>
#include <drivers/system.h>

uint64_t apic_ticks;
uint64_t apic_ms_passed;
uint64_t apic_ticks_per_ms;

void apic_enable() {
    uint32_t eax, edx;
    uint32_t msr_low, msr_high;

    __asm__ volatile ("rdmsr" : "=a"(msr_low), "=d"(msr_high) : "c"(0x1B));
    msr_low |= (1 << 11);
    __asm__ volatile ("wrmsr" : : "a"(msr_low), "d"(msr_high), "c"(0x1B));

    LAPIC_REG(LAPIC_TMRINIT) = 0;
    LAPIC_REG(LAPIC_DIV) = 0x3; // divide by 16
    LAPIC_REG(LAPIC_LVT_TMR) = APIC_TIMER_VECTOR | (0 << 16) | (1 << 17);

    LAPIC_REG(LAPIC_TMRINIT) = 0xFFFFFFFF;

    uint64_t start = pit_ms_passed;

    while (pit_ms_passed - start < 100)
        __asm__ volatile ("pause");

    uint64_t ticks = LAPIC_REG(LAPIC_TMRCURR);
    uint64_t delta = 0xFFFFFFFF - ticks;

    apic_ticks_per_ms = delta / 100;

    k_log("APIC Ticks per ms");

    char buf[32];
    itoa(apic_ticks_per_ms, buf, 16);
    k_logln(buf);

    LAPIC_REG(LAPIC_TMRINIT) = apic_ticks_per_ms; // How many times we tick before firing an interrupt.

    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void apic_timer_handler() {
    apic_ticks++;
    apic_ms_passed = apic_ticks / apic_ticks_per_ms;

    schedular_tick();
    LAPIC_REG(LAPIC_EOI) = 0;
}
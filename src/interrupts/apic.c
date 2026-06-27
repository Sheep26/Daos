#include <apic.h>
#include <thread.h>
#include <pit.h>
#include <drivers/system.h>

uint64_t apic_ticks;
uint64_t apic_ticks_per_ms;

void apic_enable() {
    LAPIC_REG(LAPIC_DIV) = 0x3;
    LAPIC_REG(LAPIC_TMRINIT) = 0xFFFFFFFF;
    LAPIC_REG(LAPIC_SVR) = 0x100 | 0xFF;

    thread_sleep_ms(1);

    LAPIC_REG(LAPIC_LVT_TMR) = 0;

    apic_ticks_per_ms = 0xFFFFFFFF - LAPIC_REG(LAPIC_TMRCURR);

    LAPIC_REG(LAPIC_LVT_TMR) = APIC_TIMER_VECTOR | (1 << 17);
    LAPIC_REG(LAPIC_DIV) = 0x3;
    LAPIC_REG(LAPIC_TMRINIT) = apic_ticks_per_ms;

    outb(0x21, 0x01);
    outb(0xA1, 0x00);

    system->cpu->apic_enabled = 1;

    char buf[32];
    itoa(apic_ticks_per_ms, buf, 10);
    k_log("APIC ticks per ms: ");
    k_logln(buf);
}

void apic_timer_handler() {
    apic_ticks++;

    schedular_tick();
    LAPIC_REG(LAPIC_EOI) = 0;
}
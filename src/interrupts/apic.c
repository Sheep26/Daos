#include <apic.h>
#include <thread.h>
#include <pit.h>
#include <drivers/system.h>

uint64_t apic_ticks;
uint64_t apic_ticks_per_ms;

void apic_enable() {
    LAPIC_REG(LAPIC_DIV) = 0x3;
    LAPIC_REG(LAPIC_TMRINIT) = 0xFFFFFFFF;
    LAPIC_REG(LAPIC_SVR) = 0x100 | 0xFF; // enable + spurious vector

    // Perform PIT-supported sleep
    thread_sleep_ms(1);

    // Stop the APIC timer
    LAPIC_REG(LAPIC_LVT_TMR) = 0;

    // Now we know how often the APIC timer has ticked in 10ms
    apic_ticks_per_ms = 0xFFFFFFFF - LAPIC_REG(LAPIC_TMRCURR);

    // Start timer as periodic on IRQ 0, divider 16, with the number of ticks we counted
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
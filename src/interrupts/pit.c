#include <pit.h>

void pit_set_frequency(uint32_t hz) {
    uint32_t divisor = PIT_BASE_FREQ / hz;

    outb(PIT_COMMAND, 0x36); // binary, mode 3, channel 0

    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

uint64_t pit_ticks = 0;
uint64_t pit_ms_passed = 0;

void pit_timer_handler(reg_t *r) {
    pit_ticks++;

    if (pit_ticks % (PIT_FREQUENCY / 1000) == 0)
        pit_ms_passed++;
    
    schedular_tick();
}
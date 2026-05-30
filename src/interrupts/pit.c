#include <pit.h>

void pit_set_frequency(uint32_t hz) {
    uint32_t divisor = PIT_BASE_FREQ / hz;

    outb(PIT_COMMAND, 0x36); // binary, mode 3, channel 0

    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}
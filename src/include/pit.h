#ifndef PIT_H
#define PIT_H

#include <drivers/io.h>
#include <stdint.h>

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND   0x43

#define PIT_BASE_FREQ 1193182

void pit_set_frequency(uint32_t hz);
void pic_remap();

#endif
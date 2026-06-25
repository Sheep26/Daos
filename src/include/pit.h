#ifndef PIT_H
#define PIT_H

#include <drivers/io.h>
#include <stdint.h>
#include <irq.h>
#include <logging.h>
#include <thread.h>

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND   0x43

#define PIT_BASE_FREQ 1193182
#define PIT_FREQUENCY 1000

extern uint64_t pit_ticks;
extern uint64_t pit_ms_passed;

void pit_timer_handler(reg_t *r);

void pit_set_frequency(uint32_t hz);
void pic_remap();

#endif
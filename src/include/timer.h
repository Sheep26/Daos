#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <drivers/system.h>

uint64_t get_ticks();
uint64_t get_ms_passed();

#endif
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <drivers/io.h>
#include <thread.h>

extern uint64_t timer_ticks;

void timer_handler();

#endif
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <drivers/io.h>
#include <thread.h>
#include <irq.h>

extern uint64_t timer_ticks;

void timer_handler(reg_t *r);

#endif
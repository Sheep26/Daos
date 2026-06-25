#include <timer.h>
#include <logging.h>
#include <pit.h>

uint64_t timer_ticks = 0;
uint64_t ms_passed = 0;

void timer_handler(reg_t *r) {
    timer_ticks++;

    if (timer_ticks % (PIT_FREQUENCY / 1000) == 0)
        ms_passed++;

    k_thread_t *t = thread_list;

    if (timer_ticks % 4 == 0)
        reschedule_needed = 1;

    while (t) {
        if (t->state == BLOCKED && t->wake_tick != 0 && timer_ticks >= t->wake_tick) {
            t->state = WAITING;

            t->wake_tick = 0;
            reschedule_needed = 1;
        }

        t = t->next;
    }
}
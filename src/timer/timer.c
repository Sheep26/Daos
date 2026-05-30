#include <timer.h>

uint64_t timer_ticks = 0;

void timer_handler(reg_t *r) {
    timer_ticks++;

    k_thread_t *t = thread_list;

    while (t) {
        if (t->state == BLOCKED && t->wake_tick != 0 && timer_ticks >= t->wake_tick) {
            t->state = WAITING;

            t->wake_tick = 0;
            reschedule_needed = 1;
        }

        t = t->next;
    }
}
#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>
#include <memory/liballoc/liballoc.h>
#include <timer.h>
#include <pit.h>

#define STACK_SIZE 4096

typedef void (*func_t) ();

struct k_thread;

enum STATE {
    ACTIVE,
    WAITING,
    BLOCKED,
    FINISHED
};

typedef struct k_thread {
    uint32_t esp;
    uint32_t ebp;

    uint8_t *stack;

    uint64_t wake_tick;

    void *function;
    enum STATE state;

    struct k_thread *next;
} k_thread_t;

extern k_thread_t *current_thread;
extern k_thread_t *idle_thread;
extern k_thread_t *thread_list;

extern int reschedule_needed;

k_thread_t *create_new_thread(void *function);
k_thread_t *create_idle_thread(void *function);

void idle_func();

void thread_block(k_thread_t *thread);
void thread_wake(k_thread_t *thread);
void thread_sleep(uint64_t ticks);
void thread_sleep_ms(uint64_t ms);

void yield();
void yield_if_needed();

void scheduler_add(k_thread_t *thread);
void scheduler_run();

#endif
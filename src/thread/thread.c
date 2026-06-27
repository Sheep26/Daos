/*
 * This thread handler is really dumb.
 * It will pick a thread and only leave it if it sleeps or finishes.
 * It won't switch between waiting threads to handle multitasking.
 * But it allows me to sleep the main thread so it is what it is.
 * It also won't free up finished threads.
*/

#include <thread.h>
#include <timer.h>
#include <drivers/system.h>

k_thread_t *thread_list = NULL;
k_thread_t *current_thread = NULL;
k_thread_t *idle_thread = NULL;
int reschedule_needed = 0;

extern void context_switch(uint32_t *old_esp, uint32_t *old_ebp, uint32_t new_esp,uint32_t new_ebp);

void idle_func() {
    while (1) {
        yield_if_needed();

        __asm__ volatile("hlt");
    }
}

void thread_bootstrap() {
    func_t fn = current_thread->function;
    fn();

    current_thread->state = FINISHED;
    yield();

    while (1);
}

void start_thread(k_thread_t *thread) {
    if (thread && thread->function) {
        thread->state = ACTIVE;

        ((func_t) thread->function)();

        thread->state = FINISHED;
    }
}

void thread_init_stack(k_thread_t *thread) {
    uint32_t *stack_top = (uint32_t *) (thread->stack + STACK_SIZE);

    // Fake return into bootstrap
    *(--stack_top) = 0; // Fake return addr
    *(--stack_top) = (uint32_t) thread_bootstrap; // Entry

    thread->esp = (uint32_t) stack_top;
    thread->ebp = (uint32_t) stack_top;
}

k_thread_t *create_new_thread(void *function) {
    k_thread_t *thread = (k_thread_t *) calloc(1, sizeof(k_thread_t));

    thread->function = (func_t) function;
    thread->stack = calloc(1, STACK_SIZE);

    thread_init_stack(thread);

    thread->state = WAITING;
    thread->wake_tick = 0;
    thread->next = NULL;

    scheduler_add(thread);
    return thread;
}

k_thread_t *create_idle_thread(void *function) {
    k_thread_t *thread = (k_thread_t *) calloc(1, sizeof(k_thread_t));

    thread->function = (func_t) function;
    thread->stack = calloc(1, STACK_SIZE);

    thread_init_stack(thread);

    thread->state = WAITING;
    thread->wake_tick = 0;
    thread->next = NULL;

    idle_thread = thread;
    return thread;
}

k_thread_t *find_next() {
    if (!current_thread)
        return idle_thread;

    k_thread_t *start = current_thread;
    k_thread_t *t;

    do {
        t = t->next ? t->next : thread_list;

        if (t->state == WAITING)
            return t;
    } while (t != start);

    return idle_thread;
}

void yield() {
    if (!current_thread)
        return;

    k_thread_t *old = current_thread;
    k_thread_t *next = find_next();

    if (old->state == ACTIVE)
        old->state = WAITING;

    if (next != idle_thread)
        next->state = ACTIVE;

    current_thread = next;
    context_switch(&old->esp, &old->ebp,next->esp, next->ebp);
}

void yield_if_needed() {
    if (reschedule_needed) {
        reschedule_needed = 0;

        yield();
    }
}

void thread_block(k_thread_t *thread) {
    if (!thread)
        return;

    thread->state = BLOCKED;
    yield();
}

void thread_wake(k_thread_t *thread) {
    if (!thread)
        return;

    if (thread->state == BLOCKED)
        thread->state = WAITING;
}

void thread_sleep(uint64_t ticks) {
    current_thread->wake_tick = get_ticks() + ticks;
    current_thread->state = BLOCKED;

    yield();
}

void thread_sleep_ms(uint64_t ms) {
    uint64_t ticks;

    if (system->cpu->apic_enabled) 
        ticks = apic_ticks_per_ms * ms;
    else
        ticks = (ms * PIT_FREQUENCY) / 1000;

    thread_sleep(ticks);
}

void scheduler_add(k_thread_t *thread) {
    if (!thread_list) {
        thread_list = thread;

        return;
    }

    k_thread_t *current = thread_list;

    while (current->next)
        current = current->next;

    current->next = thread;
}

void schedular_tick() {
    k_thread_t *t = thread_list;

    while (t) {
        if (t->state == BLOCKED && t->wake_tick != 0 && get_ticks() >= t->wake_tick) {
            t->state = WAITING;

            t->wake_tick = 0;
            reschedule_needed = 1;
        }

        t = t->next;
    }
}

void scheduler_run() {
    if (!thread_list)
        return;

    current_thread = thread_list;
    current_thread->state = ACTIVE;

    __asm__ volatile(
        "mov %0, %%esp\n"
        "mov %1, %%ebp\n"
        "ret\n"
        :
        : "r"(current_thread->esp),
          "r"(current_thread->ebp)
    );
}
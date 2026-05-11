#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <cpuid.h>
#include "io.h"

typedef struct {
    char vendor[13];
    uint32_t max_leaf;
    uint32_t max_frequency;
    uint32_t base_frequency;
} cpu_t;

void get_cpu_vendor(cpu_t *cpu);
void get_cpu_frequency_info(cpu_t *cpu);
void init_cpu(cpu_t *cpu);

static inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

#endif
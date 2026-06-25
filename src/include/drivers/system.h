#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <cpuid.h>
#include <drivers/io.h>

struct cpu;

typedef struct {
    struct cpu *cpu;

    uint64_t total_usable_ram;
} system_t;

typedef struct cpu {
    char vendor[13];
    uint32_t max_leaf;

    uint8_t apic_supported;
    uint8_t apic_enabled;
} cpu_t;

extern system_t *system;

void cpu_init(cpu_t *cpu);
void system_init();

static inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

#endif
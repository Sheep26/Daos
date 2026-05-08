#ifndef SYSTEM_H
#define SYSTEM_H

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

#endif
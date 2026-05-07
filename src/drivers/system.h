#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include <cpuid.h>
#include "io.h"

typedef struct {
    char vendor[13];
    uint32_t max_leaf;
} cpu_t;

void get_cpu_vendor(cpu_t *cpu);
void init_cpu(cpu_t *cpu);

#endif
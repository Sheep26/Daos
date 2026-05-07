#pragma once
#include <stdint.h>
#include <cpuid.h>
#include "io.h"

typedef struct {
    char vendor[13];
    uint32_t max_leaf;
} cpu_t;

cpu_t cpu;

void get_cpu_vendor(void) {
    uint32_t eax, ebx, ecx, edx;

    __cpuid(0x00, eax, ebx, ecx, edx);

    cpu.max_leaf = (uint32_t) eax;
    ((uint32_t*) cpu.vendor)[0] = ebx;
    ((uint32_t*) cpu.vendor)[1] = edx;
    ((uint32_t*) cpu.vendor)[2] = ecx;

    cpu.vendor[12] = 0;
}

void init_cpu() {
    get_cpu_vendor();
}
#include <drivers/system.h>

void get_cpu_frequency_info(cpu_t *cpu) {
    uint32_t eax, ebx, ecx, edx;

    __cpuid(0x16, eax, ebx, ecx, edx);

    cpu->max_frequency = eax;
    cpu->base_frequency = ebx;
}

void get_cpu_vendor(cpu_t *cpu) {
    uint32_t eax, ebx, ecx, edx;

    __cpuid(0x00, eax, ebx, ecx, edx);

    cpu->max_leaf = (uint32_t) eax;
    ((uint32_t*) cpu->vendor)[0] = ebx;
    ((uint32_t*) cpu->vendor)[1] = edx;
    ((uint32_t*) cpu->vendor)[2] = ecx;

    cpu->vendor[12] = 0;
}

void init_cpu(cpu_t *cpu) {
    get_cpu_vendor(cpu);
}
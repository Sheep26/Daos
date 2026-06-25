#include <drivers/system.h>

void get_cpu_vendor(cpu_t *cpu) {
    uint32_t eax, ebx, ecx, edx;

    __cpuid(0x00, eax, ebx, ecx, edx);

    cpu->max_leaf = (uint32_t) eax;
    ((uint32_t*) cpu->vendor)[0] = ebx;
    ((uint32_t*) cpu->vendor)[1] = edx;
    ((uint32_t*) cpu->vendor)[2] = ecx;

    cpu->vendor[12] = 0;
}

void apic_supported(cpu_t *cpu) {
    
}

void cpu_init(cpu_t *cpu) {
    get_cpu_vendor(cpu);
    apic_supported(cpu);
}
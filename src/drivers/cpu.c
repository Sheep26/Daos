#include <drivers/system.h>
#include <string.h>

void cpu_init(cpu_t *cpu) {
    uint32_t eax, ebx, ecx, edx;
    __cpuid(0x00, eax, ebx, ecx, edx);

    cpu->max_leaf = (uint32_t) eax;
    memcpy(&cpu->vendor[0], &ebx, 4);
    memcpy(&cpu->vendor[4], &edx, 4);
    memcpy(&cpu->vendor[8], &ecx, 4);
    cpu->vendor[12] = 0;

    __cpuid(0x1, eax, ebx, ecx, edx);

    cpu->apic_supported = (edx & (1 << 9)) != 0;
}
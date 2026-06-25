#include <timer.h>
#include <apic.h>
#include <pit.h>

uint64_t get_ticks() {
    if (system->cpu->apic_enabled)
        return apic_ticks;
    
    return pit_ticks;
}

uint64_t get_ms_passed() {
    if (system->cpu->apic_enabled)
        return apic_ms_passed;
    
    return pit_ms_passed;
}
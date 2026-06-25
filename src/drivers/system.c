#include <drivers/system.h>
#include <memory/liballoc/liballoc.h>

system_t *system;

void system_init() {
    system = (system_t*) calloc(1, sizeof(system_t));
    system->cpu = (cpu_t*) calloc(1, sizeof(cpu_t));
}
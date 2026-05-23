#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <memory/pmm.h>
#include <itoa.h>

void heap_init(uint32_t kernel_end);

#endif
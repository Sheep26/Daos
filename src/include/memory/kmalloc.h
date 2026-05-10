#ifndef KMALLOC_H
#define KMALLOC_H

#define KERNEL_HEAP_START_B 0x400000

#include <stddef.h>
#include <stdint.h>

void *kmalloc_b(size_t size);

#endif
#include <memory/kmalloc.h>

static uint8_t* heap_ptr_b = (uint8_t*) KERNEL_HEAP_START_B;

void *kmalloc_b(size_t size) {
    // kmalloc bump. please move away from this it can't access the high half of the kernal or free memory.

    void* r = heap_ptr_b;
    heap_ptr_b += size;

    return r;
}
#include <memory/heap.h>
#include <memory/liballoc/liballoc.h>
#include <drivers/io.h>
#include <utils/itoa.h>

static uint8_t* heap_ptr;
uint32_t _kernel_end;

void heap_init(uint32_t kernel_end) {
    heap_ptr = (uint8_t*) kernel_end;
    _kernel_end = kernel_end;

    serial_print("Heap initalized at: ");

    char buf[32];
    itoa(kernel_end, buf, 16);
    serial_println(buf);
}

void* liballoc_alloc(int pages) {
    void* phys = pmm_alloc_pages(pages);

    if (!phys)
        return NULL;

    void* virt = (void*) ((uint32_t) phys + _kernel_end);
    return virt;
}

int liballoc_free(void* ptr, int pages) {
    void *phys = (void*) ((uint32_t) ptr - _kernel_end);

    pmm_free_pages(phys, pages);
    return 0;
}

int liballoc_lock() {
    return 0;
}

int liballoc_unlock() {
    return 0;
}
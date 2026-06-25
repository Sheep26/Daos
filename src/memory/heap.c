#include <memory/heap.h>
#include <memory/liballoc/liballoc.h>
#include <drivers/io.h>
#include <logging.h>

uint32_t _kernel_end;

void heap_init(uint32_t kernel_end) {
    _kernel_end = kernel_end;

    k_log("Heap initalized at: ");

    char buf[32];
    itoa(kernel_end, buf, 16);
    k_logln(buf);
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
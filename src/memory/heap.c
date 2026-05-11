#include <memory/heap.h>
#include <memory/liballoc/liballoc.h>

static uint8_t* heap_ptr;

void heap_init() {
    heap_ptr = (uint8_t*) HHDM_OFFSET;
}

void* liballoc_alloc(int pages) {
    void* phys = pmm_alloc_pages(pages);

    if (!phys)
        return NULL;

    void* virt = (void*)((uint32_t)phys + HHDM_OFFSET);
    return virt;
}

int liballoc_free(void* ptr, int pages) {
    (void)ptr;
    (void)pages;

    // TODO: implement later (bitmap or free list)

    return 0;
}

int liballoc_lock() {
    return 0;
}

int liballoc_unlock() {
    return 0;
}
#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>
#include <drivers/io.h>
#include <itoa.h>
#include <string.h>

#define PAGE_SIZE 4096

#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

extern uint64_t pmm_bitmap_location;
extern uint64_t pmm_bitmap_size;

void pmm_init(multiboot_mmap_entry_t* mmap_entries, uint64_t max_addr, uint64_t mmap_entry_count, uint64_t kernal_end);
void* pmm_alloc_pages(size_t pages);
void pmm_free_pages(void* ptr, size_t pages);
void pmm_reserve_region(uint64_t start, uint64_t size);

#endif
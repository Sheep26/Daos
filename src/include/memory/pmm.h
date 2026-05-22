#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>
#include <drivers/io.h>
#include <utils/itoa.h>

#define PAGE_SIZE 4096

void pmm_init(multiboot_mmap_entry_t* mmap_entries, uint32_t max_addr, uint32_t mmap_entry_count, uint32_t kernal_end);
void* pmm_alloc_pages(size_t pages);
void pmm_free_pages(void* ptr, size_t pages);
void pmm_reserve_region(uint32_t start, uint32_t size);

#endif
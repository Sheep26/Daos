#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>
#include <drivers/io.h>

#define HHDM_OFFSET 0x400000
#define PAGE_SIZE 4096

void pmm_init(multiboot_mmap_entry_t* mmap_entries, uint32_t max_addr, uint32_t mmap_entry_count);
void* pmm_alloc_pages(size_t pages);
void pmm_free_pages(void* ptr, size_t pages);

#endif
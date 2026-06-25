#include <memory/pmm.h>

uint8_t* pmm_bitmap = 0;
uint64_t pmm_bitmap_size = 0;
uint64_t pmm_total_pages = 0;
uint64_t pmm_free_pages_count = 0;
uint64_t pmm_bitmap_location;
uint64_t pmm_bitmap_size;

void pmm_bitmap_set(uint64_t bit) {
    pmm_bitmap[bit / 8] |= (1 << (bit % 8));
}

void pmm_bitmap_clear(uint64_t bit) {
    pmm_bitmap[bit / 8] &= ~(1 << (bit % 8));
}

int pmm_bitmap_test(uint64_t bit) {
    return (pmm_bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

void pmm_init(multiboot_mmap_entry_t* mmap_entries, uint64_t max_addr, uint64_t mmap_entry_count, uint64_t kernal_end) {
    pmm_total_pages = max_addr / PAGE_SIZE;
    pmm_bitmap_size = ALIGN_UP(pmm_total_pages / 8, PAGE_SIZE);

    if (pmm_total_pages % 8 != 0) pmm_bitmap_size++;

    pmm_bitmap_location = ALIGN_UP(kernal_end, PAGE_SIZE);
    pmm_bitmap = (uint8_t*) pmm_bitmap_location;

    memset(pmm_bitmap, 0xFF, pmm_total_pages / 8);

    if (!pmm_bitmap) {
        k_logln("PMM ERROR: Could not find space for bitmap!");

        return;
    }

    // Now loop again and mark the truly usable areas as free (0)
    for (uint64_t i = 0; i < mmap_entry_count; i++) {
        if (mmap_entries[i].type == 1) {
            uint64_t start = mmap_entries[i].addr / PAGE_SIZE;
            uint64_t end = (mmap_entries[i].addr + mmap_entries[i].len) / PAGE_SIZE;

            for (uint64_t j = start; j < end; j++) {
                if (j == 0) continue; // Never allocate physical page 0 (avoids NULL pointer issues)

                pmm_bitmap_clear(j);
                pmm_free_pages_count++;
            }
        }
    }

    k_logln("Reserving kernel in memory.");
    pmm_reserve_region(0, kernal_end);

    k_logln("Reserving bitmap in memory.");
    pmm_reserve_region(pmm_bitmap_location, pmm_bitmap_size);

    k_logln("PMM Initialized successfully.");
}

void* pmm_alloc_pages(size_t pages) {
    if (pages == 0) return 0;
    
    size_t contiguous = 0;
    uint64_t start_bit = 0;
    
    // Simple first-fit search
    for (uint64_t i = 0; i < pmm_total_pages; i++)
        if (!pmm_bitmap_test(i)) {
            if (contiguous == 0) start_bit = i;
            contiguous++;

            if (contiguous == pages) {
                // Found enough pages!
                for (size_t j = 0; j < pages; j++)
                    pmm_bitmap_set(start_bit + j);

                pmm_free_pages_count -= pages;
                return (void*) (start_bit * PAGE_SIZE);
            }
        } else
            contiguous = 0;
    
    k_logln("PMM ERROR: Out of physical memory!");
    return 0; // Out of memory
}

void pmm_free_pages(void* ptr, size_t pages) {
    uint64_t start_bit = (uint64_t) ptr / PAGE_SIZE;

    for (size_t i = 0; i < pages; i++)
        pmm_bitmap_clear(start_bit + i);

    pmm_free_pages_count += pages;
}

void pmm_reserve_region(uint64_t start, uint64_t size) {
    uint64_t start_page = start / PAGE_SIZE;
    uint64_t end_page = (start + size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint64_t i = start_page; i < end_page; i++) {
        if (!pmm_bitmap_test(i)) {
            pmm_bitmap_set(i);

            pmm_free_pages_count--;
        }
    }

    k_log("Reserved memory region: ");
    
    char start_buf[32];
    char end_buf[32];
    itoa(start, start_buf, 16);
    itoa(start + size, end_buf, 16);

    k_log(start_buf);
    k_log(" - ");
    k_logln(end_buf);
}
#include <memory/pmm.h>

static uint8_t* pmm_bitmap = 0;
static uint64_t pmm_bitmap_size = 0;
static uint64_t pmm_total_pages = 0;
static uint64_t pmm_free_pages_count = 0;

static void pmm_bitmap_set(uint64_t bit) {
    pmm_bitmap[bit / 8] |= (1 << (bit % 8));
}

static void pmm_bitmap_clear(uint64_t bit) {
    pmm_bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static int pmm_bitmap_test(uint64_t bit) {
    return (pmm_bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

void pmm_init(multiboot_mmap_entry_t* mmap_entries, uint32_t max_addr, uint32_t mmap_entry_count) {
    pmm_total_pages = max_addr / PAGE_SIZE;
    pmm_bitmap_size = pmm_total_pages / 8;

    if (pmm_total_pages % 8 != 0) pmm_bitmap_size++;

    // Loop to find usable areas and place bitmap at the start.
    for (uint32_t i = 0; i < mmap_entry_count; i++) {
        if (mmap_entries[i].type == 1 && mmap_entries[i].len >= pmm_bitmap_size) {
            pmm_bitmap = (uint8_t*) (mmap_entries[i].addr + HHDM_OFFSET);

            for (uint32_t j = 0; j < pmm_bitmap_size; j++)
                pmm_bitmap[j] = 0xFF;

            // Exclude the bitmap from usable area now.
            mmap_entries[i].addr += pmm_bitmap_size;
            mmap_entries[i].len -= pmm_bitmap_size;
        }
    }

    if (!pmm_bitmap) {
        serial_print("PMM ERROR: Could not find space for bitmap!\n");

        return;
    }

    // Now loop again and mark the truly usable areas as free (0)
    for (uint32_t i = 0; i < mmap_entry_count; i++) {
        if (mmap_entries[i].type == 1) {
            uint32_t start = mmap_entries[i].addr / PAGE_SIZE;
            uint32_t end = (mmap_entries[i].addr + mmap_entries[i].len) / PAGE_SIZE;

            for (uint32_t j = start; j < end; j++) {
                if (j == 0) continue; // Never allocate physical page 0 (avoids NULL pointer issues)

                pmm_bitmap_clear(j);
                pmm_free_pages_count++;
            }
        }
    }

    serial_print("PMM Initialized successfully.\n");
}

void* pmm_alloc_pages(size_t pages) {
    if (pages == 0) return 0;
    
    size_t contiguous = 0;
    uint32_t start_bit = 0;
    
    // Simple first-fit search
    for (uint32_t i = 0; i < pmm_total_pages; i++)
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
    
    serial_print("PMM ERROR: Out of physical memory!\n");
    return 0; // Out of memory
}

void pmm_free_pages(void* ptr, size_t pages) {
    uint32_t start_bit = (uint32_t)ptr / PAGE_SIZE;

    for (size_t i = 0; i < pages; i++)
        pmm_bitmap_clear(start_bit + i);

    pmm_free_pages_count += pages;
}
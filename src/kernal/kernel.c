#include <drivers/vga.h>
#include <drivers/cpu.h>
#include <multiboot.h>
#include <utils/itoa.h>
#include <string.h>
#include <memory/pmm.h>

cpu_t cpu;

void* find_module(multiboot_tag_module_t* mod, const char* name, uint32_t* out_size) {
    if (strcmp(mod->cmdline, name) == 0) {
        if (out_size) *out_size = mod->mod_end - mod->mod_start;
        return (void*) mod->mod_start;
    }

    return 0;
}

void kernel_main(uint32_t magic, uint32_t addr) {
    if (magic != 0x36d76289) {
        // Not booted by a Multiboot-compliant loader!
        return;
    }

    serial_init();

    serial_print("Setting up multiboot\n");

    uint8_t* ptr = (uint8_t*) addr;
    uint32_t total_size = *(uint32_t*) ptr;
    ptr += 8; // skip total_size + reserved

    multiboot_tag_framebuffer_t* fb_tag = 0;

    uint32_t max_addr = 0;

    multiboot_tag_mmap_t* mmap_tag;
    multiboot_mmap_entry_t* mmap_entries;
    int mmap_entry_count = 0;

    uint64_t total_usable_ram = 0;

    while (ptr < (uint8_t*)addr + total_size) {
        multiboot_tag_t* tag = (multiboot_tag_t*) ptr;

        if (tag->type == 0) break;

        if (tag->type == MULTIBOOT_MODULE_TAG) {
            // multiboot_tag_module_t* mod = (multiboot_tag_module_t*) tag;
        }

        if (tag->type == MULTIBOOT_MMAP_TAG) {
            mmap_tag = (multiboot_tag_mmap_t*) tag;

            mmap_entries = (multiboot_mmap_entry_t*) (ptr + sizeof(multiboot_tag_mmap_t));

            mmap_entry_count = (mmap_tag->size - sizeof(multiboot_tag_mmap_t)) / mmap_tag->entry_size;

            for (int i = 0; i < mmap_entry_count; i++) {
                if (mmap_entries[i].type == 1) {
                    // usable RAM
                    total_usable_ram += mmap_entries[i].len;
                    uint32_t end = mmap_entries[i].addr + mmap_entries[i].len;

                    if (end > max_addr)
                        max_addr = end;

                    // entry[i].addr -> start
                    // entry[i].len  -> size
                }
            }
        }

        if (tag->type == MULTIBOOT_FRAMEBUFFER_TAG)
            fb_tag = (multiboot_tag_framebuffer_t*) tag;

        ptr += ((tag->size + 7) & ~7); // align to 8 bytes
    }

    if (!fb_tag) {
        serial_print("ERROR: Framebuffer uninitalized");

        while (1);
    }

    serial_print("Setup multiboot\n");

    char buf[32];
    itoa(total_usable_ram / (1024 * 1024), buf, 10);
    serial_print("Usable RAM: ");
    serial_print(buf);
    serial_print("MB");
    serial_print("\n");

    pmm_init(mmap_entries, max_addr, mmap_entry_count);
    heap_init();

    init_cpu(&cpu);

    serial_print("CPU Vendor: ");
    serial_print(cpu.vendor);
    serial_print("\n");

    setup_vga(fb_tag);

    fillscreen(0x0000FF00);
    flush_buffer();
}
#include <drivers/vga.h>
#include <drivers/cpu.h>
#include <multiboot.h>
#include <utils/itoa.h>

#define VGA_BUFFER 0xb8000

cpu_t cpu;
tty_t tty;

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

    multiboot_tag_framebuffer_t* fb_tag;
    uint64_t total_usable_ram = 0;

    while (ptr < (uint8_t*)addr + total_size) {
        multiboot_tag_t* tag = (multiboot_tag_t*) ptr;

        if (tag->type == 0) break;

        if (tag->type == MULTIBOOT_MMAP_TAG) {
            multiboot_tag_mmap_t* mmap_tag = (multiboot_tag_mmap_t*) tag;

            multiboot_mmap_entry_t* entry =
                (multiboot_mmap_entry_t*)(ptr + sizeof(multiboot_tag_mmap_t));

            int entries = (mmap_tag->size - sizeof(multiboot_tag_mmap_t)) / mmap_tag->entry_size;

            for (int i = 0; i < entries; i++) {
                if (entry[i].type == 1) {
                    // usable RAM
                    total_usable_ram += entry[i].len;

                    // entry[i].addr -> start
                    // entry[i].len  -> size
                }
            }
        }

        if (tag->type == MULTIBOOT_FRAMEBUFFER_TAG)
            fb_tag = (multiboot_tag_framebuffer_t*) tag;

        ptr += ((tag->size + 7) & ~7); // align to 8 bytes
    }

    serial_print("Setup multiboot\n");

    char buf[32];
    itoa(total_usable_ram / (1024 * 1024), buf, 10);
    serial_print("Usable RAM: ");
    serial_print(buf);
    serial_print("MB");
    serial_print("\n");

    init_cpu(&cpu);

    serial_print("CPU Vendor: ");
    serial_print(cpu.vendor);
    serial_print("\n");

    setup_vga(fb_tag);

    fillscreen(0x0000FF00);
    fillrect(100, 100, 200, 200, 0x00FF0000);
}
#include <drivers/vga.h>
#include <drivers/cpu.h>
#include <multiboot.h>
#include <utils/itoa.h>
#include <video.h>
#include <string.h>

#define VGA_BUFFER 0xb8000

cpu_t cpu;
tty_t tty;

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
    uint64_t total_usable_ram = 0;

    uint32_t video_size = 0;
    uint8_t* video_data = 0;

    while (ptr < (uint8_t*)addr + total_size) {
        multiboot_tag_t* tag = (multiboot_tag_t*) ptr;

        if (tag->type == 0) break;

        if (tag->type == MULTIBOOT_MODULE_TAG) {
            multiboot_tag_module_t* mod = (multiboot_tag_module_t*) tag;

            video_data = find_module(mod, "video", &video_size);
        }

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

    init_cpu(&cpu);

    serial_print("CPU Vendor: ");
    serial_print(cpu.vendor);
    serial_print("\n");

    setup_vga(fb_tag);

    const uint32_t frame_width = video_width;
    const uint32_t frame_height = video_height;

    const uint32_t bytes_per_row = (frame_width + 7) / 8;
    const uint32_t bytes_per_frame = bytes_per_row * frame_height;

    uint32_t num_frames = video_size / bytes_per_frame;

    for (uint32_t frame = 0; frame < num_frames; frame++) {
        fillscreen(0x00000000);

        for (uint32_t y = 0; y < frame_height; y++) {
            for (uint32_t byte = 0; byte < bytes_per_row; byte++) {
                uint8_t row_byte = video_data[frame * bytes_per_frame + y * bytes_per_row + byte];

                for (uint32_t bit = 0; bit < 8; bit++) {
                    uint32_t x = byte * 8 + bit;
                    if (x >= frame_width) break; // last byte might have padding bits

                    uint8_t pixel = (row_byte >> (7 - bit)) & 1;

                    putpixel(x, y, pixel ? 0x00FFFFFF : 0x00000000);
                }
            }
        }

        flush_buffer();
    }
}
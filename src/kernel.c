#include <drivers/io.h>
#include <drivers/vga.h>
#include <drivers/system.h>
#include <multiboot.h>

#define VGA_BUFFER 0xb8000

cpu_t cpu;
tty_t tty;
multiboot_info_t* mbi;

void kernel_main(uint32_t magic, uint32_t addr) {
    if (magic != 0x2BADB002) {
        // Not booted by a Multiboot-compliant loader!
        return;
    }

    serial_init();
    init_cpu(&cpu);

    serial_print("CPU Vendor: ");
    serial_print(cpu.vendor);
    serial_print("\n");

    tty = (tty_t) {0, 0, WIDTH, HEIGHT, (uint16_t*) VGA_BUFFER};
    mbi = (multiboot_info_t*) addr;

    setup_vga(mbi);
    fillscreen(0x00FF0000);
}
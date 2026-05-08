#include <drivers/io.h>
#include <drivers/vga.h>
#include <drivers/system.h>
#include <multiboot.h>

#define VGA_BUFFER 0xb8000

cpu_t cpu;
tty_t tty;
multiboot_info_t* mbi;

uint32_t* framebuffer;
uint32_t pitch;

char* itoa(int value, char* str, int base) {
    /* 
    @param int value - The value of the int.
    @param char* str - The buffer for the output.
    @param int base - The base of the int (ie normal numbers would be base 10)
    */
    char* rc;
    char* ptr;
    char* low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }

    return rc;
}

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

    serial_print("Checking framebuffer available\n");

    // Check framebuffer available.
    if (!(mbi->flags & (1 << 12))) {
        serial_print("ERROR: Framebuffer not available\n");
        while (1);
    }

    serial_print("Framebuffer available\n");

    char buf[32];

    serial_print("VGA Width: ");
    itoa(mbi->framebuffer_width, buf, 10);
    serial_print(buf);
    serial_print("\n");

    serial_print("VGA Height: ");
    itoa(mbi->framebuffer_height, buf, 10);
    serial_print(buf);
    serial_print("\n");

    serial_print("VGA Pitch: ");
    itoa(mbi->framebuffer_pitch, buf, 10);
    serial_print(buf);
    serial_print("\n");

    serial_print("VGA BPP: ");
    itoa(mbi->framebuffer_bpp, buf, 10);
    serial_print(buf);
    serial_print("\n");

    serial_print("VGA TYPE: ");
    itoa(mbi->framebuffer_type, buf, 10);
    serial_print(buf);
    serial_print("\n");

    framebuffer = (uint32_t*)(uintptr_t) mbi->framebuffer_addr;
    pitch = (uint32_t) mbi->framebuffer_pitch;

    for (uint32_t y = 0; y < mbi->framebuffer_height; y++) {
        for (uint32_t x = 0; x < mbi->framebuffer_width; x++) {
            putpixel(x, y, 0x00FF0000, framebuffer, pitch);
        }
    }
}
#include "drivers/io.h"
#include "drivers/vga.h"
#include "drivers/system.h"

volatile uint16_t *vga_buffer = (uint16_t*) VGA_BUFFER;

cpu_t cpu;
vga_t vga;

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

void kernel_main() {
    vga = (vga_t) {0, 0, WIDTH, HEIGHT, vga_buffer};

    serial_init();
    init_cpu(&cpu);

    print(cpu.vendor, 0xF, &vga);
}
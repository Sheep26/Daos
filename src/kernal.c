#include <stdint.h>
#include "io.h"

#define VGA_BUFFER 0xb8000
#define WIDTH 80
#define HEIGHT 25

volatile uint16_t *vga_buffer = (uint16_t*) VGA_BUFFER;

typedef struct {
	uint16_t vga_x;
	uint16_t vga_y;
	uint16_t vga_width;
	uint16_t vga_height;
} VGA;

VGA vga = {0, 0, WIDTH, HEIGHT};

void print(const char *str, uint8_t attrib) {
	while (*str) {
		uint16_t vga_pos = vga.vga_y * vga.vga_width + vga.vga_x;
		vga_buffer[vga_pos] = (attrib << 8) | *str++;

		vga.vga_x++;

		if (vga.vga_x >= vga.vga_width) {
			vga.vga_x = 0;
			vga.vga_y ++;
		}
	}
}

void clearln(uint8_t ln) {
	for (uint8_t x = 0; x < vga.vga_width; x++)
		vga_buffer[ln * vga.vga_width + x] = (0x00 << 8) | ' ';
}

void scroll() {
	for (uint8_t y = 0; y < vga.vga_height - 1; y++)
		for (uint8_t x = 0; x < vga.vga_width; x++)
			vga_buffer[y * vga.vga_width + x] =
				vga_buffer[(y + 1) * vga.vga_width + x];
}

void println(const char *str, uint8_t attrib) {
	print(str, attrib);

	vga.vga_x = 0;
	vga.vga_y++;

	if (vga.vga_y >= vga.vga_height) {
		scroll();
		clearln(vga.vga_height - 1);
		vga.vga_y = vga.vga_height - 1;
	}
}

char * itoa(int value, char* str, int base) {
    char * rc;
    char * ptr;
    char * low;
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

void main() {
	println("Woo", 0xF);
}
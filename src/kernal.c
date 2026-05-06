#include <stdint.h>
#include "io.h"

#define VGA_BUFFER 0xb8000
#define WIDTH 80
#define HEIGHT 25

volatile uint16_t *vga_buffer = (uint16_t*) VGA_BUFFER;
uint16_t vga_x = 0;
uint16_t vga_y = 0;

void print(const char *str, uint8_t attrib) {
	while (*str) {
		uint16_t vga_pos = vga_y * WIDTH + vga_x;
		vga_buffer[vga_pos] = (attrib << 8) | *str++;

		vga_x++;

		if (vga_x >= WIDTH) {
			vga_x = 0;
			vga_y++;
		}
	}
}

void clearln(uint8_t ln) {
	for (uint8_t x = 0; x < WIDTH; x++)
		vga_buffer[ln * WIDTH + x] = (0x00 << 8) | ' ';
}

void scroll() {
	for (uint8_t y = 0; y < HEIGHT; y++)
		for (uint8_t x = 0; x < WIDTH; x++)
			vga_buffer[y * WIDTH + x] = vga_buffer[(y + 1) * WIDTH + x];
}

void println(const char *str, uint8_t attrib) {
	print(str, attrib);

	vga_x = 0;
	vga_y++;

	if (vga_y >= HEIGHT) {
		clearln(vga_y - HEIGHT);

		scroll();
	}
}

void main() {
	while (1) {
		while (!(inb(0x64) & 1));

		uint8_t scancode = inb(0x60);

		if (scancode & 0x80)
			continue;

		println("Hello world", 0xF);
	}
}
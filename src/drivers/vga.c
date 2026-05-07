#include "vga.h"

void print(const char *str, uint8_t attrib, vga_t *vga) {
	while (*str) {
		uint16_t vga_pos = vga->vga_y * vga->vga_width + vga->vga_x;
		vga->vga_buffer[vga_pos] = (attrib << 8) | *str++;

		vga->vga_x++;

		if (vga->vga_x >= vga->vga_width) {
			vga->vga_x = 0;
			vga->vga_y ++;
		}
	}
}

void clearln(uint8_t ln, vga_t *vga) {
	for (uint8_t x = 0; x < vga->vga_width; x++)
		vga->vga_buffer[ln * vga->vga_width + x] = (0x00 << 8) | ' ';
}

void scroll(vga_t *vga) {
	for (uint8_t y = 0; y < vga->vga_height - 1; y++)
		for (uint8_t x = 0; x < vga->vga_width; x++)
			vga->vga_buffer[y * vga->vga_width + x] =
				vga->vga_buffer[(y + 1) * vga->vga_width + x];
}

void println(const char *str, uint8_t attrib, vga_t *vga) {
	print(str, attrib, vga);

	vga->vga_x = 0;
	vga->vga_y++;

	if (vga->vga_y >= vga->vga_height) {
		scroll(vga);
		clearln(vga->vga_height - 1, vga);
		vga->vga_y = vga->vga_height - 1;
	}
}
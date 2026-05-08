#include <drivers/vga.h>

void print(const char *str, uint8_t attrib, tty_t *tty) {
	while (*str) {
		uint16_t vga_pos = tty->tty_y * tty->tty_width + tty->tty_x;
		tty->vga_buffer[vga_pos] = (attrib << 8) | *str++;

		tty->tty_x++;

		if (tty->tty_x >= tty->tty_width) {
			tty->tty_x = 0;
			tty->tty_y ++;
		}
	}
}

void clearln(uint8_t ln, tty_t *tty) {
	for (uint8_t x = 0; x < tty->tty_width; x++)
		tty->vga_buffer[ln * tty->tty_width + x] = (0x00 << 8) | ' ';
}

void scroll(tty_t *tty) {
	for (uint8_t y = 0; y < tty->tty_height - 1; y++)
		for (uint8_t x = 0; x < tty->tty_width; x++)
			tty->vga_buffer[y * tty->tty_width + x] =
				tty->vga_buffer[(y + 1) * tty->tty_width + x];
}

void println(const char *str, uint8_t attrib, tty_t *tty) {
	print(str, attrib, tty);

	tty->tty_x = 0;
	tty->tty_y++;

	if (tty->tty_y >= tty->tty_height) {
		scroll(tty);
		clearln(tty->tty_height - 1, tty);
		tty->tty_y = tty->tty_height - 1;
	}
}

void putpixel(int x, int y, uint32_t color, uint32_t *framebuffer, uint32_t pitch) {
    uint32_t pitch_pixels = pitch / 4;

    framebuffer[y * pitch_pixels + x] = color;
}
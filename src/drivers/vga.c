#include <drivers/vga.h>
#include <drivers/io.h>

vga_t vga;

void print_tty(const char *str, uint8_t attrib, tty_t *tty) {
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

void clearln_tty(uint8_t ln, tty_t *tty) {
	for (uint8_t x = 0; x < tty->tty_width; x++)
		tty->vga_buffer[ln * tty->tty_width + x] = (0x00 << 8) | ' ';
}

void scroll_tty(tty_t *tty) {
	for (uint8_t y = 0; y < tty->tty_height - 1; y++)
		for (uint8_t x = 0; x < tty->tty_width; x++)
			tty->vga_buffer[y * tty->tty_width + x] =
				tty->vga_buffer[(y + 1) * tty->tty_width + x];
}

void println_tty(const char *str, uint8_t attrib, tty_t *tty) {
	print_tty(str, attrib, tty);

	tty->tty_x = 0;
	tty->tty_y++;

	if (tty->tty_y >= tty->tty_height) {
		scroll_tty(tty);
		clearln_tty(tty->tty_height - 1, tty);
		tty->tty_y = tty->tty_height - 1;
	}
}

void setup_vga(multiboot_info_t* mbi) {
	serial_print("Setting up VGA\n");
	serial_print("Checking framebuffer availability\n");

    // Check framebuffer available.
    if (!(mbi->flags & (1 << 12))) {
        serial_print("ERROR: Framebuffer not available\n");
        while (1);
    }

    serial_print("Framebuffer is available\n");

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

	vga = (vga_t) {mbi, (uint32_t*)(uintptr_t) mbi->framebuffer_addr, mbi->framebuffer_pitch};
	serial_print("VGA setup complete\n");
}

void putpixel(int x, int y, uint32_t colour) {
    uint32_t pitch_pixels = vga.pitch / 4;

    vga.framebuffer[y * pitch_pixels + x] = colour;
}

void fillscreen(uint32_t colour) {
	for (uint32_t y = 0; y < vga.mbi->framebuffer_height; y++)
        for (uint32_t x = 0; x < vga.mbi->framebuffer_width; x++)
            putpixel(x, y, colour);
}

void fillrect(int x, int y, int width, int height, uint32_t colour) {
	for (int i = 0; i < width; i++)
		for (int z = 0; z < height; z++)
			putpixel(x + i, y + z, colour);
}

void flush_vga() {

}
#include <drivers/vga.h>

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

void setup_vga(multiboot_tag_framebuffer_t* fb_tag) {
	serial_println("Setting up VGA");

    char buf[32];

    serial_print("VGA Width: ");
    itoa(fb_tag->framebuffer_width, buf, 10);
    serial_println(buf);

    serial_print("VGA Height: ");
    itoa(fb_tag->framebuffer_height, buf, 10);
    serial_println(buf);

    serial_print("VGA Pitch: ");
    itoa(fb_tag->framebuffer_pitch, buf, 10);
    serial_println(buf);

    serial_print("VGA BPP: ");
    itoa(fb_tag->framebuffer_bpp, buf, 10);
    serial_println(buf);

    serial_print("VGA TYPE: ");
    itoa(fb_tag->framebuffer_type, buf, 10);
    serial_println(buf);

	vga.framebuffer_addr = fb_tag->framebuffer_addr;
	vga.framebuffer = (uint32_t*) fb_tag->framebuffer_addr;
	vga.framebuffer_pitch = fb_tag->framebuffer_pitch;
	vga.framebuffer_width = fb_tag->framebuffer_width;
	vga.framebuffer_height = fb_tag->framebuffer_height;
	vga.framebuffer_bpp = fb_tag->framebuffer_bpp;
	vga.framebuffer_type = fb_tag->framebuffer_type;

	uint32_t *backbuffer = calloc(1, vga.framebuffer_height * vga.framebuffer_pitch);
	vga.backbuffer = backbuffer;

	serial_println("VGA setup complete");
}

void putpixel(int x, int y, uint32_t colour) {
    uint32_t bpp = vga.framebuffer_bpp / 8;
    uint32_t pitch_pixels = vga.framebuffer_pitch / bpp;

    if (x < 0 || y < 0 ||
        x >= (int) vga.framebuffer_width ||
        y >= (int) vga.framebuffer_height)
        return;

    vga.backbuffer[y * pitch_pixels + x] = colour;
}

void fillscreen(uint32_t colour) {
	for (uint32_t y = 0; y < vga.framebuffer_height; y++)
        for (uint32_t x = 0; x < vga.framebuffer_width; x++)
            putpixel(x, y, colour);
}

void fillrect(int x, int y, int width, int height, uint32_t colour) {
	for (int i = 0; i < width; i++)
		for (int z = 0; z < height; z++)
			putpixel(x + i, y + z, colour);
}

void draw_char(char c, int px, int py, uint32_t fg, uint32_t bg, font_t font) {
    uint8_t *glyph = font.font_data + ((uint8_t) c * font.font_height);

    for (int y = 0; y < font.font_height; y++) {
        uint8_t row = glyph[y];

        for (int x = 0; x < font.font_width; x++) {
            if (row & (1 << x))
                putpixel(px + x, py + y, fg);
            else
                putpixel(px + x, py + y, bg);
        }
    }
}

void draw_string(const char *s, int x, int y, uint32_t fg, uint32_t bg, font_t font) {
    while (*s++) {
        draw_char(*(s - 1), x, y, fg, bg, font);

		x += font.font_width;
	}
}

void flush_buffer() {
    for (uint32_t pixel = 0; pixel < vga.framebuffer_height * (vga.framebuffer_pitch/4); pixel++)
		vga.framebuffer[pixel] = vga.backbuffer[pixel];
}
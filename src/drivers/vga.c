#include <drivers/vga.h>
#include <fs/vfs.h>
#include <drivers/tty.h>

vga_t vga;
tty_t *tty;

void setup_tty(font_t font) {
	tty = malloc(sizeof(tty));

	tty->tty_width = WIDTH;
	tty->tty_height = HEIGHT;
	tty->tty_x = 0;
	tty->tty_y = 0;
	tty->font = font;
	tty_init();
}

void reset_tty() {
	tty->tty_x = 0;
	tty->tty_y = 0;
	tty_init();
}

void newline_tty() {
	tty->tty_x = 0;
	tty->tty_y++;

	if (tty->tty_y > tty->tty_height) {
		scroll_tty(tty);
		tty->tty_y = tty->tty_height - 1;
	}
}

void backspace_tty() {
	if (tty->tty_x > strlen(cwd) + 1) {
		tty->tty_x--;

		for (int y = 0; y < tty->font.font_height; y++)
			for (int x = 0; x < tty->font.font_width; x++)
				putpixel(tty->tty_x * tty->font.font_width + x, tty->tty_y * tty->font.font_height + y, 0x00000000);

		flush_buffer();
	}
}

void print_tty(const char *str) {
	while (*str++) {
		int progress = 1;
		char c = *(str - 1);
		
		if (c == '\n') {
			newline_tty();

			progress = 0;
		}

		draw_char(c, tty->tty_x * tty->font.font_width, tty->tty_y * tty->font.font_height, 0x00FFFFFF, 0x00000000, tty->font);
		flush_buffer();

		if (progress)
			tty->tty_x++;

		if (tty->tty_x >= tty->tty_width)
			newline_tty();
	}
}

void clearln_tty(uint16_t ln) {
    for (uint16_t y = 0; y < tty->font.font_height; y++)
        for (uint16_t x = 0; x < tty->tty_width * tty->font.font_width; x++)
            putpixel(x, ln * tty->font.font_height + y, 0x00000000);

    flush_buffer();
}

void scroll_tty(tty_t *tty) {
    int line_height = tty->font.font_height;
    int screen_width = tty->tty_width * tty->font.font_width;
    int screen_height = tty->tty_height * line_height;

    // Move all lines up by 1 line_height
    for (int y = 0; y < screen_height - line_height; y++)
        for (int x = 0; x < screen_width; x++)
            // Copy pixel from the line below
            putpixel(x, y, getpixel(x, y + line_height));

    // Clear the last line
    for (int y = screen_height - line_height; y < screen_height; y++)
        for (int x = 0; x < screen_width; x++)
            putpixel(x, y, 0x00000000);

    flush_buffer();
}

void println_tty(const char *str) {
	print_tty(str);

	tty->tty_x = 0;
	tty->tty_y++;

	if (tty->tty_y >= tty->tty_height) {
		scroll_tty(tty);
		// clearln_tty(tty->tty_height - 1);

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

uint32_t getpixel(int x, int y) {
    uint32_t bpp = vga.framebuffer_bpp / 8;
    uint32_t pitch_pixels = vga.framebuffer_pitch / bpp;

	return vga.framebuffer[y * pitch_pixels + x];
}

void putpixel(int x, int y, uint32_t colour) {
    uint32_t bpp = vga.framebuffer_bpp / 8;
    uint32_t pitch_pixels = vga.framebuffer_pitch / bpp;

    if (x < 0 || y < 0 || x >= (int) vga.framebuffer_width || y >= (int) vga.framebuffer_height)
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
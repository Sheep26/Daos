#ifndef VGA_H
#define VGA_H

#define WIDTH 80
#define HEIGHT 25

#include <stdint.h>

enum video_type {
	VIDEO_TYPE_NONE = 0x00,
	VIDEO_TYPE_COLOUR = 0x20,
	VIDEO_TYPE_MONOCHROME = 0x30,
};

typedef struct {
	uint16_t tty_x;
	uint16_t tty_y;
	uint16_t tty_width;
	uint16_t tty_height;
	volatile uint16_t *vga_buffer;
} tty_t;

void print(const char *str, uint8_t attrib, tty_t *vga);
void clearln(uint8_t ln, tty_t *vga);
void scroll(tty_t *vga);
void println(const char *str, uint8_t attrib, tty_t *vga);

void putpixel(int x, int y, uint32_t color, uint32_t *framebuffer, uint32_t pitch);

#endif
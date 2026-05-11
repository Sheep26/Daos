#ifndef VGA_H
#define VGA_H

#define WIDTH 80
#define HEIGHT 25

#include <stdint.h>
#include <multiboot.h>
#include <utils/itoa.h>
#include <drivers/io.h>
#include <memory/liballoc/liballoc.h>

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

typedef struct {
	uint64_t framebuffer_addr;
	uint32_t framebuffer_pitch;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	uint32_t framebuffer_bpp;
	uint32_t framebuffer_type;
	uint32_t *framebuffer;
	uint32_t *backbuffer;
} vga_t;

void print_tty(const char *str, uint8_t attrib, tty_t *vga);
void clearln_tty(uint8_t ln, tty_t *vga);
void scroll_tty(tty_t *vga);
void println_tty(const char *str, uint8_t attrib, tty_t *vga);

void setup_vga(multiboot_tag_framebuffer_t* fb_tag);

void putpixel(int x, int y, uint32_t colour);
void fillscreen(uint32_t colour);
void fillrect(int x, int y, int width, int height, uint32_t colour);
void flush_buffer();

#endif
#ifndef VGA_H
#define VGA_H

#define WIDTH 80
#define HEIGHT 60

#include <stdint.h>
#include <multiboot.h>
#include <itoa.h>
#include <drivers/io.h>
#include <memory/liballoc/liballoc.h>
#include <font.h>

typedef struct {
	uint16_t tty_x;
	uint16_t tty_y;
	uint16_t tty_width;
	uint16_t tty_height;
	font_t font;
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

void setup_tty(font_t font);

void print_tty(const char *str);
void clearln_tty(uint16_t ln);
void scroll_tty();
void println_tty(const char *str);
void backspace_tty();

void setup_vga(multiboot_tag_framebuffer_t* fb_tag);

uint32_t getpixel(int x, int y);

void putpixel(int x, int y, uint32_t colour);
void fillscreen(uint32_t colour);
void fillrect(int x, int y, int width, int height, uint32_t colour);

void draw_char(char c, int px, int py, uint32_t fg, uint32_t bg, font_t font);
void draw_string(const char* s, int x, int y, uint32_t fg, uint32_t bg, font_t font);

void flush_buffer();

#endif
#ifndef VGA_H
#define VGA_H

#define VGA_BUFFER 0xb8000
#define WIDTH 80
#define HEIGHT 25

#include <stdint.h>

typedef struct {
	uint16_t vga_x;
	uint16_t vga_y;
	uint16_t vga_width;
	uint16_t vga_height;
	volatile uint16_t *vga_buffer ;
} vga_t;

void print(const char *str, uint8_t attrib, vga_t *vga);
void clearln(uint8_t ln, vga_t *vga);
void scroll(vga_t *vga);
void println(const char *str, uint8_t attrib, vga_t *vga);

#endif
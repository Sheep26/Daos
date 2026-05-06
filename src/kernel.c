#include <stdint.h>

#define VGA_BUFFER 0xb8000

void print_string(const char *s, uint8_t attrib)
{
	volatile uint16_t *vp = (uint16_t *)VGA_BUFFER;

	while (*s)
		*vp++ = (attrib << 8) | *s++;
}

void main() {
    print_string("Hello World\0", 0xF);
}
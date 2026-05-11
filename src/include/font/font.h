#ifndef FONT_H
#define FONT_H

#include <stdint.h>

typedef struct {
    int font_width;
    int font_height;
    uint8_t *font_data;
} font_t;

extern font_t font8x8_basic;

#endif
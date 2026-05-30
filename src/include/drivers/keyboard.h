#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <reg.h>

#define KEYBOARD_DATA 0x60
#define KEYBOARD_CONTROL 0x64

typedef struct {
    unsigned char key;
    int press;
} kb_t;

typedef struct {
    kb_t item[16];

    int f;
    int b;
    int cnt;
} queue_t;

void keyboard_handler(reg_t *r);
char keyboard_key();

#endif
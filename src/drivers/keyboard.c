#include <drivers/keyboard.h>
#include <drivers/io.h>

// US keyboard layout
unsigned char kbdus[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

unsigned char shift_kbdus[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
 '"', '~',   0,		/* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

volatile int shift_press = 0;
volatile queue_t q = {.f = 0, .b = 0};
kb_t null_kb = {.press = 0, .key = 0};

void push(unsigned char key, int press) {
    if(q.cnt >= 15) return;

    q.item[q.b].key = key;
    q.item[q.b].press = press;
    
    q.b = (q.b + 1) % 16;
    q.cnt++;
}

kb_t pop() {
    if (q.b == q.f) return null_kb;

    kb_t i = q.item[q.f];
    q.f = (q.f + 1) % 16;

    q.cnt--;
    return i;
}

char keyboard_key() {
    while(1) {
        if(q.cnt > 0){
            kb_t k = pop();

            if(k.key != 0 && k.press == 1)
                return k.key;
        }
    }
}

void flush_keyboard() {
    while (inb(KEYBOARD_CONTROL) & KEYBOARD_OBF)
        (void) inb(KEYBOARD_DATA);  // discard old key
}

void keyboard_handler(reg_t *r) {
    uint8_t scancode = inb(KEYBOARD_DATA);

    serial_println("Rah");

    if((scancode & 0x7F) == 0x2A || (scancode & 0x7F) == 0x36){
        shift_press = (scancode & 0x80) ? 0 : 1;

        return;
    }

    unsigned char k = shift_press ? shift_kbdus[scancode & 0x7F] : kbdus[scancode & 0x7F];
    push(k, scancode & 0x80 ? 0 : 1);
}
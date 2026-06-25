#include <logging.h>

void log(char *s) {
    serial_println(s);
    println_tty(s);
}
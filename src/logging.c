#include <logging.h>

void k_log(char *s) {
    serial_println(s);
    println_tty(s);
}
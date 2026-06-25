#include <logging.h>

uint8_t log_tty;

void k_log(char *s) {
    serial_println(s);

    if (log_tty)
        println_tty(s);
}
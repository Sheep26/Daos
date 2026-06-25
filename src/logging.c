#include <logging.h>

uint8_t log_tty;

void k_log(char *s) {
    serial_print(s);

    if (log_tty)
        print_tty(s);
}

void k_logln(char *s) {
    serial_println(s);

    if (log_tty)
        println_tty(s);
}
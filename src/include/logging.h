#ifndef LOGGING_H
#define LOGGING_H

#include <drivers/io.h>
#include <drivers/vga.h>

extern uint8_t log_tty;

void k_log(char *s);

#endif
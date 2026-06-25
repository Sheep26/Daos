#ifndef TTY_H
#define TTY_H

#define TTY_BUF_SIZE 1024
extern char tty_input[TTY_BUF_SIZE];

void tty_input_handler(char c);
void tty_reset();

#endif
#include <drivers/tty.h>
#include <drivers/vga.h>
#include <fs/vfs.h>
#include <command_handler.h>

char tty_input[TTY_BUF_SIZE] = {0};

void tty_input_handler(char c) {
    char buf[2] = {c, '\0'};
    size_t len = strlen(tty_input);

    switch (c) {
        case '\b':
            if (len > 0)
                tty_input[len - 1] = '\0';

            backspace_tty();
            break;
        case '\n':
            print_tty(buf);
            char cpyied[TTY_BUF_SIZE];
            memcpy(cpyied, tty_input, sizeof(tty_input));
            exec_command(cpyied);

            memset(tty_input, '\0', sizeof(tty_input));
            print_tty(cwd);
            print_tty(" ");

            break;
        default:
            if (len < TTY_BUF_SIZE - 1) {
                tty_input[len] = c;     // add new char at the end
                tty_input[len + 1] = '\0'; // terminate string
            }

            print_tty(buf);
            break;
    }
}
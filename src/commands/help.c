#include <commands.h>
#include <command_handler.h>

void run_help(char *argv[], int argc) {
    command_t *pos = command_list;
    println_tty("Commands");

    while (pos) {
        newline_tty();
        print_tty(pos->name);
        print_tty(" - ");
        println_tty(pos->description);

        pos = pos->next;
    }
}
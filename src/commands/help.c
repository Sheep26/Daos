#include <commands.h>
#include <command_handler.h>

void run_help() {
    println_tty("Commands");

    command_t *pos = command_list;

    while (pos) {
        newline_tty();
        print_tty(pos->name);
        print_tty(" - ");
        println_tty(pos->description);

        pos = pos->next;
    }
}
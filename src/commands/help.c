#include <commands.h>
#include <command_handler.h>

void run_help() {
    newline_tty();
    println_tty("Commands");

    command_t *pos = command_list;

    while (pos) {
        println_tty(pos->name);

        pos = pos->next;
    }
}
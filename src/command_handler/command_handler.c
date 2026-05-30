#include <command_handler.h>
#include <string.h>
#include <drivers/io.h>
#include <drivers/vga.h>

command_t *command_list = NULL;

command_t *create_command(char *name, func_t function) {
    command_t *command = (command_t*) calloc(1, sizeof(command_t));

    command->name = calloc(strlen(name) + 1, 1);
    strcpy(command->name, name);

    command->function = function;

    if (!command_list) {
        command_list = command;

        return command;
    }

    command_t *pos = command_list;

    while (pos->next)
        pos = pos->next;
    
    pos->next = command;
    return command;
}

command_t *find_command(char *name) {
    command_t *pos = command_list;

    while (pos) {
        if (strcmp(pos->name, name) == 0)
            return pos;

        pos = pos->next;
    }

    return NULL;
}

void exec_command(char cmd[]) {
    int i = 0;

    // skip leading spaces
    while (cmd[i] == ' ') i++;

    // command start
    char *name = &cmd[i];

    // find end of command
    while (cmd[i] != ' ' && cmd[i] != '\0') i++;

    // terminate command in-place
    if (cmd[i] != '\0') {
        cmd[i] = '\0';
        i++;
    }

    // args start (skip spaces)
    while (cmd[i] == ' ') i++;

    char *args = &cmd[i];

    command_t *command = find_command(name);

    if (!command) {
        print_tty("Command not found: ");

        println_tty(name);
        return;
    }

    command->function(args);
}
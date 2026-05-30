#include <command_handler.h>
#include <string.h>
#include <drivers/io.h>
#include <drivers/vga.h>
#include <itoa.h>

command_t *command_list = NULL;

void dump_commands() {
    command_t *pos = command_list;
    char buf[32];

    serial_println("Dump");
    serial_print("cmd_list head: ");
    itoa((uint32_t)command_list, buf, 10);
    serial_println(buf);

    while (pos) {
        serial_print("Command: ");

        itoa(pos->magic, buf, 16);
        serial_println(buf);

        pos = pos->next;
    }
}

command_t *create_command(char *name, char *description, cmd_func_t function) {
    command_t *command = (command_t*) malloc(sizeof(command_t));

    command->name = malloc(strlen(name) + 1);
    command->description = malloc(strlen(description) + 1);

    strcpy(command->name, name);
    strcpy(command->description, description);

    command->function = function;
    command->next = NULL;

    command->magic = CMD_MAGIC;

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
    dump_commands();
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

    char *argv[MAX_ARGS];
    int argc = 0;

    char *saveptr;
    char *token = strtok_r(args, " ", &saveptr);

    while (token && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok_r(NULL, " ", &saveptr);
    }

    command_t *command = find_command(name);

    if (!command) {
        print_tty("Command not found: ");

        println_tty(name);
        return;
    }

    command->function(argv, argc);
}
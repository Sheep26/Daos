#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <stdint.h>

#define MAX_ARGS 32

typedef void (*func_t) ();
struct command_s;

typedef struct command_s {
    char *name;
    char *description;

    func_t function;
    struct command_s *next;
} command_t;

extern command_t *command_list;

command_t *create_command(char *name, char *description, func_t function);
void exec_command(char cmd[]);

#endif
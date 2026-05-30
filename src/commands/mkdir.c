#include <commands.h>
#include <fs/vfs.h>

void run_mkdir(char *argv[], int argc) {
    if (argc != 1) {
        println_tty("mkdir expects 1 argument");

        return;
    }

    mkdir_fs(argv[0], 0);
}
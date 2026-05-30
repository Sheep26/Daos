#include <commands.h>
#include <fs/vfs.h>

void run_cat(char *argv[], int argc) {
    if (argc != 1) {
        println_tty("cat expects 1 argument");

        return;
    }

    fs_node_t *file = kopen(argv[0], 0);

    if (file) {
        char *buf = calloc(1, file->length);
        read_fs(file, 0, file->length, buf);

        println_tty(buf);
        return;
    }

    println_tty("File not found");
}
#include <commands.h>
#include <fs/vfs.h>

void run_cd(char *argv[], int argc) {
    if (argc != 1) {
        println_tty("cd expects 1 argument");
        return;
    }

    fs_node_t *file = kopen(argv[0], 0);

    if (file)
        cwd = canonicalize_path(cwd, argv[0]);
    else
        println_tty("Path not found");
    
    close_fs(file);
    free(file);
}
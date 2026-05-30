#include <commands.h>
#include <itoa.h>
#include <fs/vfs.h>

void run_ls(char *argv[], int argc) {
    if (!argc) {
        fs_directory_t dir;
        ls_fs(NULL, &dir);

        print_tty("Index of: ");
        println_tty(cwd);

        println_tty("Name - Size - Is dir");
        newline_tty();

        for (int i = 0; i < dir.count; i++) {
            fs_directory_node_t node = (fs_directory_node_t) dir.nodes[i];

            print_tty(node.name);
            print_tty(" - ");

            char buf[32];
            itoa(node.size, buf, 10);
            print_tty(buf);

            print_tty(" - ");
            itoa(node.is_dir, buf, 10);
            println_tty(buf);
        }

        return;
    }

    if (argc > 1) {
        println_tty("ls excpects 1 argument");
        return;
    }

    char *wd = argv[0];

    fs_directory_t dir;
    ls_fs(wd, &dir);

    print_tty("Index of: ");
    println_tty(wd);

    println_tty("Name - Size - Is dir");
    newline_tty();

    for (int i = 0; i < dir.count; i++) {
        fs_directory_node_t node = (fs_directory_node_t) dir.nodes[i];

        print_tty(node.name);
        print_tty(" - ");

        char buf[32];
        itoa(node.size, buf, 10);
        print_tty(buf);

        print_tty(" - ");
        itoa(node.is_dir, buf, 10);
        println_tty(buf);
    }
}
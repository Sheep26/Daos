#ifndef BADAPPLE_H
#define BADAPPLE_H

#include <drivers/vga.h>
#include <fs/vfs.h>
#include <font.h>
#include <drivers/io.h>

#define TARGET_FPS 24 // Double speed.

void run_badapple(char *args);
void run_help(char *args);
void run_ls(char *args);

#endif
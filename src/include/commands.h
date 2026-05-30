#ifndef BADAPPLE_H
#define BADAPPLE_H

#include <drivers/vga.h>
#include <fs/vfs.h>
#include <font.h>
#include <drivers/io.h>

#define TARGET_FPS 24 // Double speed.

void run_badapple(char *argv[], int argc);
void run_help(char *argv[], int argc);
void run_ls(char *argv[], int argc);
void run_mkdir(char *argv[], int argc);
void run_cat(char *argv[], int argc);

#endif
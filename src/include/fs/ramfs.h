#ifndef RAMFS_H
#define RAMFS_H

#include <stdint.h>
#include <fs/vfs.h>

typedef struct ramfs_file {
    char name[256];
    uint8_t *data;
    uint32_t size;
    uint8_t is_dir;
} ramfs_file_t;

typedef struct ramfs {
    ramfs_file_t *files;
    uint32_t count;
    uint32_t capacity;
} ramfs_t;

fs_node_t *ramfs_create(void);

#endif
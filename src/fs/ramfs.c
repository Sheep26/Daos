#include <fs/ramfs.h>
#include <memory/heap.h>
#include <string.h>

ramfs_file_t *ramfs_find(ramfs_t *fs, char *name) {
    for (uint32_t i = 0; i < fs->count; i++) {
        if (!strcmp(fs->files[i].name, name))
            return &fs->files[i];
    }
    return NULL;
}

struct dirent *ramfs_readdir(fs_node_t *node, uint32_t index) {
    ramfs_t *fs = node->device;

    if (index >= fs->count)
        return NULL;

    struct dirent *d = malloc(sizeof(struct dirent));
    memset(d, 0, sizeof(struct dirent));

    strcpy(d->name, fs->files[index].name);
    d->ino = index;

    return d;
}

fs_node_t *ramfs_finddir(fs_node_t *node, char *name) {
    ramfs_t *fs = node->device;

    ramfs_file_t *f = ramfs_find(fs, name);
    if (!f) return NULL;

    fs_node_t *out = malloc(sizeof(fs_node_t));
    memset(out, 0, sizeof(fs_node_t));

    strcpy(out->name, f->name);

    out->flags = f->is_dir ? VFS_DIR : VFS_FILE;
    out->length = f->size;

    out->device = f;

    if (f->is_dir) {
        out->readdir = ramfs_readdir;
        out->finddir = ramfs_finddir;
    } else {
        out->read = NULL; // you can add later
    }

    return out;
}

int ramfs_create_file(fs_node_t *node, char *name, void *data, uint32_t size, uint16_t perm) {
    ramfs_t *fs = node->device;

    if (ramfs_find(fs, name))
        return 0;

    if (fs->count >= fs->capacity)
        return 0;

    ramfs_file_t *f = &fs->files[fs->count++];

    memset(f, 0, sizeof(ramfs_file_t));
    strcpy(f->name, name);

    f->size = size;
    f->is_dir = 0;

    if (size) {
        f->data = malloc(size);
        memcpy(f->data, data, size);
    }

    return 1;
}

uint32_t ramfs_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    ramfs_file_t *f = node->device;

    if (offset >= f->size)
        return 0;

    if (offset + size > f->size)
        size = f->size - offset;

    memcpy(buffer, f->data + offset, size);
    return size;
}

fs_node_t *ramfs_create(void) {
    fs_node_t *root = malloc(sizeof(fs_node_t));
    memset(root, 0, sizeof(fs_node_t));

    strcpy(root->name, "ramfs");
    root->flags = VFS_DIR;

    ramfs_t *fs = malloc(sizeof(ramfs_t));
    memset(fs, 0, sizeof(ramfs_t));

    fs->capacity = 128;
    fs->files = malloc(sizeof(ramfs_file_t) * fs->capacity);

    root->device = fs;

    root->readdir = ramfs_readdir;
    root->finddir = ramfs_finddir;
    root->create = ramfs_create_file;

    return root;
}
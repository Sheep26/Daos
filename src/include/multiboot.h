#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_MMAP_TAG 6
#define MULTIBOOT_FRAMEBUFFER_TAG 8

typedef struct multiboot_tag {
    uint32_t type;
    uint32_t size;
} multiboot_tag_t;

typedef struct multiboot_tag_framebuffer {
    uint32_t type;
    uint32_t size;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
} multiboot_tag_framebuffer_t;

typedef struct multiboot_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
} multiboot_tag_mmap_t;

typedef struct multiboot_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t reserved;
} multiboot_mmap_entry_t;

#endif
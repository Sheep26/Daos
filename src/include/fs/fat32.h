#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>
#include <stddef.h>
#include <drivers/ata.h>
#include <memory/liballoc/liballoc.h>
#include <string.h>
#include <itoa.h>
#include <fs/vfs.h>

#define MAX_NODES 128
#define ATTR_DIR 0x10
#define ATTR_FILE 0x20

typedef struct __attribute__((packed)) {
    uint8_t jump[3];
    char oem[8];

    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t root_entries;
    uint16_t total_sectors_16;
    uint8_t media_type;
    uint16_t sectors_per_fat_16;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;

    uint32_t sectors_per_fat_32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
    uint16_t fs_info;
    uint16_t backup_boot_sector;

    uint8_t reserved[12];

    uint8_t drive_number;
    uint8_t reserved1;
    uint8_t boot_signature;
    uint32_t volume_id;

    char volume_label[11];
    char fs_type[8];

    uint8_t boot_code[420];

    uint16_t signature;
} bpb_t;

typedef struct __attribute__((packed)) {
    char name[11];
    uint8_t attr;
    uint8_t nt_res;
    uint8_t creation_time_tenth;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_low;
    uint32_t size;
} directory_entry_t;

typedef struct {
    char name[13];
    uint32_t size;
    int is_dir;
    uint32_t cluster;
} dir_info_t;

typedef struct __attribute__((packed)) {
    uint32_t lead_signature;
    uint8_t  reserved1[480];
    uint32_t struct_signature;
    uint32_t free_count;
    uint32_t next_free;
    uint8_t  reserved2[12];
    uint32_t trail_signature;
} fsinfo_t;

typedef struct {
    ata_t *ata;
    bpb_t *bpb;

    uint32_t *fat;
    uint32_t fat_start;
    uint32_t fat_sectors;
} fat32_disk_t;

typedef struct {
    char name[16];
    uint32_t cluster;
    uint32_t dir_cluster;
    uint32_t size;
    uint8_t is_dir;
} fat_directory_node_t;

typedef struct {
    fat_directory_node_t nodes[MAX_NODES];
    int count;
} fat_directory_t;

typedef struct {
    fat32_disk_t *disk;
    uint32_t cluster;
    uint32_t parent_cluster;
    uint32_t size;
} fat_node_t;

int fat_load(fat32_disk_t *fat32_disk);
void fat_init(fat32_disk_t *fat32_disk);
void fat_disk_init(fat32_disk_t *fat32_disk, ata_t *ata);
void fat_flush(fat32_disk_t *fat32_disk);

int fat_format(fat32_disk_t *fat32_disk, char *label);

int fat_write_file(fat32_disk_t *fat32_disk, char *name, void *data, uint32_t size, uint32_t dir_cluster);

uint32_t fat_read_file(fat32_disk_t *disk, char *name, void *out_buffer, uint32_t dir_cluster);
uint32_t fat_file_size(fat32_disk_t *disk, char *name, uint32_t dir_cluster);
uint32_t fat_file_exists(fat32_disk_t *disk, uint32_t dir_cluster, char *name);

int fat_find_file(fat32_disk_t *disk, uint32_t dir_cluster, char *name, directory_entry_t *out);
void fat_ls(fat32_disk_t *disk, uint32_t dir_cluster, fat_directory_t *out);
int fat_mkdir(fat32_disk_t *disk, uint32_t parent_cluster, char *name);

int fat_delete_file(fat32_disk_t *disk, uint32_t dir_cluster, char *name);

fs_node_t *fat_mount_create(fat32_disk_t *disk, char *name);

fs_node_t *fat_vfs_finddir(fs_node_t *node, char *name);
struct dirent *fat_vfs_readdir(fs_node_t *node, uint32_t index);
uint32_t fat_vfs_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);

int fat_vfs_ls(fs_node_t *node, fs_directory_t *out);

int fat_vfs_create_file(fs_node_t *node, char *name, void *data, uint32_t size, uint16_t permission);
int fat_vfs_mkdir(fs_node_t *node, char *name, uint16_t permission);

int fat_vfs_rm(fs_node_t *node, char *name);

#endif
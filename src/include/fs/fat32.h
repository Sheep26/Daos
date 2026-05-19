#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>
#include <stddef.h>
#include <drivers/ata.h>
#include <memory/liballoc/liballoc.h>
#include <string.h>
#include <utils/itoa.h>

#define MAX_FILES 128

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
    uint32_t lead_signature;   // 0x00
    uint8_t  reserved1[480];   // 0x04
    uint32_t struct_signature; // 0x1E4
    uint32_t free_count;       // 0x1E8
    uint32_t next_free;        // 0x1EC
    uint8_t  reserved2[12];    // 0x1F0
    uint32_t trail_signature;  // 0x1FC
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
    uint32_t size;
    uint8_t is_dir;
} fs_node_t;

typedef struct {
    fs_node_t files[MAX_FILES];
    int count;
} fs_list_t;

int fat_load(fat32_disk_t *fat32_disk);
void fat_init(fat32_disk_t *fat32_disk);
void fat_flush(fat32_disk_t *fat32_disk);
uint32_t cluster_to_lba(fat32_disk_t *fat32_disk, uint32_t cluster);

void fat_disk_init(fat32_disk_t *fat32_disk, ata_t *ata);
int fat_find_file(fat32_disk_t *disk, uint32_t dir_cluster, char *name, directory_entry_t *out);

void fat_format_name(char *in, char out[11]);

int format(fat32_disk_t *fat32_disk, char *label);

int create_directory_entry(fat32_disk_t *fat32_disk, uint32_t dir_cluster, char *name, uint32_t first_cluster, uint32_t size);
int find_dir_slot(fat32_disk_t *fat32_disk, directory_entry_t *entries);

uint32_t write_file(fat32_disk_t *fat32_disk, void *data, uint32_t size);
void write_cluster(fat32_disk_t *fat32_disk, uint32_t cluster, void *data);
int fs_write_file(fat32_disk_t *fat32_disk, char *name, void *data, uint32_t size, uint32_t dir_cluster);

void read_cluster(fat32_disk_t *disk, uint32_t cluster, void *buffer);
uint32_t fs_read_file(fat32_disk_t *disk, char *name, void *out_buffer, uint32_t dir_cluster);

void fs_ls(fat32_disk_t *disk, uint32_t dir_cluster, fs_list_t *out);

#endif
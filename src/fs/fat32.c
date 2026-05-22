#include <fs/fat32.h>

static void *memset16(uint16_t *buf, uint16_t val, size_t count) {
    for (size_t i = 0; i < count; i++)
        buf[i] = val;

    return buf;
}

static uint8_t choose_spc(uint32_t total_sectors) {
    uint8_t spc = 1;

    while (spc <= 128) {
        uint32_t clusters = total_sectors / spc;

        if (clusters >= 65525)
            return spc;

        spc <<= 1;
    }

    return 128;
}

int find_dir_slot(fat32_disk_t *fat32_disk, directory_entry_t *entries) {
    int total = (fat32_disk->bpb->sectors_per_cluster * 512) / sizeof(directory_entry_t);

    for (int i = 0; i < total; i++)
        if (entries[i].name[0] == 0x00 || entries[i].name[0] == 0xE5)
            return i;

    return -1;
}

int create_directory_entry(fat32_disk_t* fat32_disk, uint32_t dir_cluster, char *name, uint32_t first_cluster, uint32_t size, uint8_t attr) {
    uint32_t dir_lba = cluster_to_lba(fat32_disk, dir_cluster);

    uint8_t *sector = malloc(fat32_disk->bpb->sectors_per_cluster * 512);
    read_sectors(dir_lba, fat32_disk->bpb->sectors_per_cluster, (uint16_t*) sector, fat32_disk->ata);

    directory_entry_t *entries = (directory_entry_t*) sector;

    int slot = find_dir_slot(fat32_disk, entries);

    if (slot < 0) {
        free(sector);

        return 0;
    }

    directory_entry_t *e = &entries[slot];

    fat_format_name(name, e->name);

    e->attr = attr;

    e->first_cluster_low = (uint16_t) (first_cluster & 0xFFFF);
    e->first_cluster_high = (uint16_t) (first_cluster >> 16);

    e->size = size;

    write_sectors(dir_lba, fat32_disk->bpb->sectors_per_cluster, (uint16_t*) sector, fat32_disk->ata);

    free(sector);
    return 1;
}

int fat_find_file(fat32_disk_t *disk, uint32_t dir_cluster, char *name, directory_entry_t *out) {
    uint32_t lba = cluster_to_lba(disk, dir_cluster);

    uint8_t *sector = malloc(disk->bpb->sectors_per_cluster * 512);

    read_sectors(lba, disk->bpb->sectors_per_cluster, (uint16_t*)sector, disk->ata);

    directory_entry_t *entries = (directory_entry_t*)sector;

    char formatted[11];
    fat_format_name(name, formatted);

    for (uint32_t i = 0; i < (disk->bpb->sectors_per_cluster * 512) / sizeof(directory_entry_t); i++) {
        if (entries[i].name[0] == 0x00)
            break;

        if (memcmp(entries[i].name, formatted, 11) == 0) {
            if (entries[i].name[0] == 0xE5) // Deleted files
                continue;

            if (entries[i].attr == 0x0F) // Deleted files
                continue;

            *out = entries[i];

            free(sector);
            return 1;
        }
    }

    free(sector);
    return 0;
}

void read_cluster(fat32_disk_t *disk, uint32_t cluster, void *buffer) {
    uint32_t lba = cluster_to_lba(disk, cluster);

    read_sectors(lba, disk->bpb->sectors_per_cluster, (uint16_t*)buffer, disk->ata);
}

uint32_t fs_file_size(fat32_disk_t *disk, char *name, uint32_t dir_cluster) {
    directory_entry_t e;

    if (!fat_find_file(disk, dir_cluster, name, &e))
        return 0;

    return e.size;
}

uint32_t fs_file_exists(fat32_disk_t *disk, char *name, uint32_t dir_cluster) {
    directory_entry_t e;

    return fat_find_file(disk, dir_cluster, name, &e);
}

uint32_t fs_read_file(fat32_disk_t *disk, char *name, void *out_buffer, uint32_t dir_cluster) {
    directory_entry_t e;

    if (!fat_find_file(disk, dir_cluster, name, &e))
        return 0;

    uint32_t cluster = (e.first_cluster_high << 16) | e.first_cluster_low;

    uint32_t cluster_size = disk->bpb->sectors_per_cluster * 512;

    uint8_t *out = (uint8_t*) out_buffer;

    uint32_t remaining = e.size;

    while (cluster < 0x0FFFFFF8 && remaining > 0) {
        read_cluster(disk, cluster, out);

        cluster = disk->fat[cluster];

        out += cluster_size;
        remaining = (remaining > cluster_size) ? remaining - cluster_size : 0;
    }

    return e.size;
}

void fat_format_name(char *in, char out[11]) {
    for (int i = 0; i < 11; i++)
        out[i] = ' ';

    int i = 0, j = 0;

    // name
    while (in[i] && in[i] != '.' && j < 8)
        out[j++] = in[i++];

    // skip '.'
    while (in[i] && in[i] != '.') i++;

    if (in[i] == '.') i++;

    j = 8;

    // extension
    int k = 0;
    while (in[i] && k < 3)
        if (j + k < 11)
            out[j + k++] = in[i++];
}

int fat_load(fat32_disk_t *fat32_disk) {
    uint32_t *fat = malloc(fat32_disk->fat_sectors * 512);

    if (!read_sectors(fat32_disk->fat_start, fat32_disk->fat_sectors, (uint16_t*) fat, fat32_disk->ata))
        return 0;

    fat32_disk->fat = fat;
    return 1;
}

void fat_disk_init(fat32_disk_t *fat32_disk, ata_t *ata) {
    fat32_disk->ata = ata;

    uint8_t sector[512];

    if (!read_sectors(0, 1, (uint16_t*)sector, ata)) {
        serial_print("Drive: ATA");

        char buf[32];
        itoa(fat32_disk->ata->identifier, buf, 10);

        serial_print(buf);
        serial_println("Failed to read boot sector");

        return;
    }

    fat32_disk->bpb = malloc(sizeof(bpb_t));

    if (!fat32_disk->bpb) {
        serial_println("Failed to allocate BPB");

        return;
    }

    // Copy bpb from sector.
    memcpy(fat32_disk->bpb, sector, sizeof(bpb_t));

    // Validate.
    if (fat32_disk->bpb->signature != 0xAA55) {
        serial_println("Invalid FAT32 signature");

        free(fat32_disk->bpb);
        fat32_disk->bpb = NULL;

        return;
    }

    if (fat32_disk->bpb->sectors_per_fat_32 == 0) {
        serial_println("Not a FAT32 filesystem");

        free(fat32_disk->bpb);
        fat32_disk->bpb = NULL;

        return;
    }

    fat_init(fat32_disk);
}

void fat_init(fat32_disk_t *fat32_disk) {
    fat32_disk->fat_start = fat32_disk->bpb->reserved_sectors;
    fat32_disk->fat_sectors = fat32_disk->bpb->sectors_per_fat_32;

    fat_load(fat32_disk);
}

uint32_t fat_find_free(uint32_t *fat, uint32_t total_clusters) {
    for (uint32_t i = 3; i < total_clusters; i++)
        if (fat[i] == 0x00000000)
            return i;

    return 0;
}

uint32_t fat_alloc_cluster(fat32_disk_t* fat32_disk) {
    uint32_t total_clusters = (fat32_disk->ata->sectors / fat32_disk->bpb->sectors_per_cluster);

    for (uint32_t i = 3; i < total_clusters; i++)
        if (fat32_disk->fat[i] == 0x00000000)
            return i;

    return 0;
}

uint32_t cluster_to_lba(fat32_disk_t *fat32_disk, uint32_t cluster) {
    uint32_t first_data_sector = fat32_disk->bpb->reserved_sectors + (fat32_disk->bpb->fat_count * fat32_disk->bpb->sectors_per_fat_32);

    return first_data_sector + ((cluster - 2) * fat32_disk->bpb->sectors_per_cluster);
}

void write_cluster(fat32_disk_t *fat32_disk, uint32_t cluster, void *data) {
    uint32_t lba = cluster_to_lba(fat32_disk, cluster);

    write_sectors(lba, fat32_disk->bpb->sectors_per_cluster, (uint16_t*) data, fat32_disk->ata);
}

uint32_t write_file(fat32_disk_t *fat32_disk, void *data, uint32_t size) {
    uint32_t cluster_size_bytes = fat32_disk->bpb->sectors_per_cluster * 512;

    uint32_t clusters_needed = (size + cluster_size_bytes - 1) / cluster_size_bytes;

    uint32_t first_cluster = 0;
    uint32_t prev_cluster = 0;

    uint8_t *ptr = (uint8_t*) data;

    for (uint32_t i = 0; i < clusters_needed; i++) {
        uint32_t cluster = fat_alloc_cluster(fat32_disk);

        if (!cluster) return 0;

        if (!first_cluster)
            first_cluster = cluster;

        if (prev_cluster)
            fat32_disk->fat[prev_cluster] = cluster;

        uint32_t write_size = (size > cluster_size_bytes) ? cluster_size_bytes : size;

        write_cluster(fat32_disk, cluster, ptr);

        ptr += write_size;
        size -= write_size;

        prev_cluster = cluster;
    }

    fat32_disk->fat[prev_cluster] = 0x0FFFFFFF;

    fat_flush(fat32_disk);
    return first_cluster;
}

int fs_write_file(fat32_disk_t *fat32_disk, char *name, void *data, uint32_t size, uint32_t dir_cluster) {
    uint32_t cluster = write_file(fat32_disk, data, size);

    if (!cluster || !size)
        return 0;

    create_directory_entry(fat32_disk, dir_cluster, name, cluster, size, ATTR_FILE);
    return 1;
}

void fat_flush(fat32_disk_t *fat32_disk) {
    write_sectors(fat32_disk->bpb->reserved_sectors, fat32_disk->bpb->sectors_per_fat_32, (uint16_t*) fat32_disk->fat, fat32_disk->ata);
    write_sectors(fat32_disk->bpb->reserved_sectors + fat32_disk->bpb->sectors_per_fat_32, fat32_disk->bpb->sectors_per_fat_32, (uint16_t*) fat32_disk->fat, fat32_disk->ata);
}

void fat_to_string(char raw[11], char *out) {
    int i = 0, j = 0;

    // name part (0–7)
    for (i = 0; i < 8; i++) {
        if (raw[i] == ' ')
            break;
        out[j++] = raw[i];
    }

    // extension?
    int has_ext = 0;
    for (int k = 8; k < 11; k++) {
        if (raw[k] != ' ') {
            has_ext = 1;
            break;
        }
    }

    if (has_ext) {
        out[j++] = '.';

        for (i = 8; i < 11; i++) {
            if (raw[i] == ' ')
                break;
            out[j++] = raw[i];
        }
    }

    out[j] = '\0';
}

void add_fs_list_entry(directory_t *directory, directory_entry_t *e, uint32_t dir_cluster) {
    if (directory->count >= MAX_NODES)
        return;

    directory_node_t *n = &directory->nodes[directory->count++];

    fat_to_string(e->name, n->name);

    n->cluster = (e->first_cluster_high << 16) | e->first_cluster_low;
    n->dir_cluster = dir_cluster;
    n->size = e->size;
    n->is_dir = (e->attr & 0x10) ? 1 : 0;
}

void fs_ls(fat32_disk_t *disk, uint32_t dir_cluster, directory_t *out) {
    out->count = 0;
    uint32_t cluster = dir_cluster;
    uint32_t cluster_size = disk->bpb->sectors_per_cluster * 512;

    uint8_t *buffer = malloc(cluster_size);

    while (cluster < 0x0FFFFFF8 && cluster != 0) {
        read_cluster(disk, cluster, buffer);

        directory_entry_t *entries = (directory_entry_t *)buffer;

        int count = cluster_size / sizeof(directory_entry_t);

        for (int i = 0; i < count; i++) {
            if (entries[i].name[0] == 0x00)
                return;

            if (entries[i].name[0] == 0xE5)
                continue;

            if (entries[i].attr == 0x0F)
                continue;

            add_fs_list_entry(out, &entries[i], dir_cluster);
        }

        cluster = disk->fat[cluster];
    }

    free(buffer);
}

int fs_mkdir(fat32_disk_t *disk, uint32_t parent_cluster, char *name) {
    // Allocate cluster for new directory
    uint32_t cluster = fat_alloc_cluster(disk);

    if (!cluster)
        return 0;

    // Mark end-of-chain
    disk->fat[cluster] = 0x0FFFFFFF;

    fat_flush(disk);

    uint32_t cluster_size = disk->bpb->sectors_per_cluster * 512;

    // Clear directory cluster
    uint8_t *buffer = calloc(1, cluster_size);

    if (!buffer)
        return 0;

    directory_entry_t *entries = (directory_entry_t *) buffer;

    //
    // "." entry
    //
    fat_format_name(".", entries[0].name);

    entries[0].attr = ATTR_DIR;

    entries[0].first_cluster_low = (uint16_t) (cluster & 0xFFFF);

    entries[0].first_cluster_high = (uint16_t) (cluster >> 16);

    //
    // ".." entry
    //
    fat_format_name("..", entries[1].name);

    entries[1].attr = ATTR_DIR;

    entries[1].first_cluster_low = (uint16_t) (parent_cluster & 0xFFFF);

    entries[1].first_cluster_high = (uint16_t) (parent_cluster >> 16);

    // Write new directory cluster
    write_cluster(disk, cluster, buffer);

    free(buffer);

    // Add entry into parent directory
    if (!create_directory_entry(disk, parent_cluster, name, cluster, 0, ATTR_DIR))
        return 0;

    return 1;
}

int format(fat32_disk_t *fat32_disk, char *label) {
    if (sizeof(bpb_t) != 512) {
        serial_println("BPB BAD SIZE");

        return 0;
    }

    if (sizeof(fsinfo_t) != 512) {
        serial_println("FSINFO BAD SIZE");

        return 0;
    }

    uint16_t bootbuf[256];
    memset16(bootbuf, 0, 256);

    bpb_t *bpb = (bpb_t*) bootbuf;

    bpb->jump[0] = 0xEB;
    bpb->jump[1] = 0x3C;
    bpb->jump[2] = 0x90;

    char oem[8] = OEM_NAME;

    for (int i = 0; i < 8; i++)
        bpb->oem[i] = oem[i];

    bpb->bytes_per_sector = 512;
    bpb->sectors_per_cluster = choose_spc(fat32_disk->ata->sectors);
    bpb->reserved_sectors = 32;
    bpb->fat_count = 2;
    bpb->media_type = 0xF8;

    bpb->total_sectors_16 = 0;
    bpb->total_sectors_32 = fat32_disk->ata->sectors;

    bpb->sectors_per_fat_16 = 0;
    bpb->sectors_per_fat_32 = 0;

    bpb->root_cluster = 2;

    bpb->fs_info = 1;
    bpb->backup_boot_sector = 6;

    bpb->ext_flags = 0;
    bpb->fs_version = 0;

    bpb->sectors_per_track = 63;
    bpb->head_count = 255;
    bpb->hidden_sectors = 0;

    bpb->drive_number = 0x80;
    bpb->reserved1 = 0;
    bpb->boot_signature = 0x29;
    bpb->volume_id = 0x12345678;

    for (int i = 0; i < 11; i++)
        bpb->volume_label[i] =
            (label && label[i]) ? label[i] : ' ';

    for (int i = 0; i < 8; i++)
        bpb->fs_type[i] = "FAT32   "[i];
    
    /*bpb->boot_code[0] = 0xFA; // cli
    bpb->boot_code[1] = 0xF4; // hlt
    bpb->boot_code[2] = 0xEB; // jmp short
    bpb->boot_code[3] = 0xFD; // jump backwards forever*/

    bpb->boot_code[0] = 0xFA; // cli
    bpb->boot_code[1] = 0xEB; // jmp short
    bpb->boot_code[2] = 0xFE; // jump to self

    uint32_t fat_size = 1;
    uint32_t total_clusters = 0;

    while (1) {
        uint32_t data_sectors = fat32_disk->ata->sectors - bpb->reserved_sectors - (bpb->fat_count * fat_size);

        total_clusters = data_sectors / bpb->sectors_per_cluster;

        uint32_t new_fat_size = ((total_clusters + 2) * 4 + (bpb->bytes_per_sector - 1)) / bpb->bytes_per_sector;
            //((total_clusters + 2) * 4 + 511) / 512;

        if (new_fat_size == fat_size)
            break;

        fat_size = new_fat_size;
    }

    if (total_clusters < 65525)
        return 0;
    
    if (fat_size == 0)
        return 0;

    bpb->sectors_per_fat_32 = fat_size;

    // boot signature
    bpb->signature = 0xAA55;

    uint16_t zero[256];
    memset16(zero, 0, 256);

    uint32_t total_meta = bpb->reserved_sectors + (bpb->fat_count * fat_size);

    for (uint32_t i = 1; i < total_meta; i++)
        if (!write_sectors(i, 1, zero, fat32_disk->ata))
            return 0;

    if (!write_sectors(0, 1, bootbuf, fat32_disk->ata))
        return 0;

    if (!write_sectors(6, 1, bootbuf, fat32_disk->ata))
        return 0;

    uint32_t fat_entries[128]; // 512 bytes

    for (int i = 0; i < 128; i++)
        fat_entries[i] = 0;

    fat_entries[0] = 0x0FFFFFF8; // media descriptor
    fat_entries[1] = 0xFFFFFFFF;
    fat_entries[2] = 0x0FFFFFFF; // root cluster

    // write FAT #1
    if (!write_sectors(bpb->reserved_sectors, 1, (uint16_t*) fat_entries, fat32_disk->ata))
        return 0;

    // write FAT #2
    if (!write_sectors(bpb->reserved_sectors + fat_size, 1, (uint16_t*) fat_entries, fat32_disk->ata))
        return 0;
    
    uint32_t first_data_sector = bpb->reserved_sectors + (bpb->fat_count * fat_size);

    uint32_t root_sector = first_data_sector + ((bpb->root_cluster - 2) * bpb->sectors_per_cluster);

    for (uint32_t i = 0; i < bpb->sectors_per_cluster; i++)
        if (!write_sectors(root_sector + i, 1, zero, fat32_disk->ata))
            return 0;

    uint16_t fsbuf[256];
    memset16(fsbuf, 0, 256);

    fsinfo_t *fs = (fsinfo_t*) fsbuf;

    fs->lead_signature = 0x41615252;
    fs->struct_signature = 0x61417272;
    fs->free_count = 0xFFFFFFFF;
    fs->next_free = 3;
    fs->trail_signature = 0xAA550000;

    if (!write_sectors(1, 1, fsbuf, fat32_disk->ata))
        return 0;

    if (!write_sectors(7, 1, fsbuf, fat32_disk->ata))
        return 0;

    fat32_disk->bpb = malloc(sizeof(bpb_t));
    memcpy(fat32_disk->bpb, bpb, sizeof(bpb_t));

    fat_init(fat32_disk);

    serial_print("Formatted disk: ");
    serial_println(label);

    return 1;
}
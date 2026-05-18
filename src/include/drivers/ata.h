#ifndef ATA_H
#define ATA_H

#define ATA_PRIMARY_DATA 0x1F0
#define ATA_PRIMARY_ERR 0x1F1
#define ATA_PRIMARY_SECCOUNT 0x1F2
#define ATA_PRIMARY_LBA_LOW 0x1F3
#define ATA_PRIMARY_LBA_MID 0x1F4
#define ATA_PRIMARY_LBA_HIGH 0x1F5
#define ATA_PRIMARY_DRIVE_SEL 0x1F6
#define ATA_PRIMARY_COMMAND 0x1F7
#define ATA_PRIMARY_STATUS 0x1F7

#define OEM_NAME "DaOS"

#include <stdint.h>
#include <utils/itoa.h>
#include <drivers/io.h>

typedef struct {
    // Idenifier
    int identifier;

    // Pointers
    uint16_t data;
    uint16_t err;
    uint16_t seccount;
    uint16_t lba_low;
    uint16_t lba_mid;
    uint16_t lba_high;
    uint16_t drive_sel;
    uint16_t command;
    uint16_t status;

    // Metadata
    uint16_t *metadata;
    char model[41];
    int slave;
    uint32_t sectors;
} ata_t;

int wait_bsy(ata_t *ata);
int ata_exists(ata_t *ata);
int wait_drq(ata_t *ata);
int read_sectors(uint32_t lba, uint8_t count, uint16_t *buffer, ata_t *ata);
int write_sectors(uint32_t lba, uint8_t count, uint16_t *buffer, ata_t *ata);
int ata_identify(ata_t *ata);
void init_ata(ata_t *ata, uint16_t data, uint16_t err, uint16_t seccount, uint16_t lba_low, uint16_t lba_mid, uint16_t lba_high, uint16_t drive_sel, uint16_t command, uint16_t status, int slave);

#endif
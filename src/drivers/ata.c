#include <drivers/ata.h>

int ata_ids = 0;

void ata_io_wait(ata_t *ata) {
    for (int i = 0; i < 4; i++) inb(ata->status);
}

int wait_bsy(ata_t *ata) {
    uint32_t timeout = 1000000;

    while (timeout--) {
        uint8_t status = inb(ata->status);

        if (status == 0xFF)
            return 0;

        if (!(status & 0x80))
            return 1;
    }

    return 0;
}

int wait_drq(ata_t *ata) {
    uint32_t timeout = 1000000;

    while (timeout--) {
        uint8_t status = inb(ata->status);

        if (status == 0xFF) return 0;
        if (!(status & 0x80) && (status & 0x08)) return 1;

        if (status & 0x01) return 0; // Error
        if (status & 0x20) return 0; // Drive Fault
    }

    return 0;
}

int ata_exists(ata_t *ata) {
    uint8_t sel = ata->slave ? 0xB0 : 0xA0;

    outb(ata->drive_sel, sel);

    for(int i=0; i<10; i++) ata_io_wait(ata); // Wait a bit longer for selection
    
    uint8_t status = inb(ata->status);
    
    return status != 0xFF;
}

void init_ata(ata_t *ata, uint16_t data, uint16_t err, uint16_t seccount, uint16_t lba_low, uint16_t lba_mid, uint16_t lba_high, uint16_t drive_sel, uint16_t command, uint16_t status, int slave) {
    ata->identifier = ata_ids;
    ata->data = data;
    ata->err = err;
    ata->seccount = seccount;
    ata->lba_low = lba_low;
    ata->lba_mid = lba_mid;
    ata->lba_high = lba_high;
    ata->drive_sel = drive_sel;
    ata->command = command;
    ata->status = status;
    ata->slave = slave;

    ata_ids++;
}

int ata_identify(ata_t *ata) {
    if (!ata_exists(ata)) return 0;

    outb(ata->drive_sel, ata->slave ? 0xB0 : 0xA0);
    ata_io_wait(ata);

    outb(ata->seccount, 0);
    outb(ata->lba_low, 0);
    outb(ata->lba_mid, 0);
    outb(ata->lba_high, 0);
    outb(ata->command, 0xEC);
    ata_io_wait(ata);

    if (inb(ata->status) == 0)
        return 0;

    if (!wait_bsy(ata)) {
        serial_print("[ATA] Identify: Timeout waiting for BSY\n");

        return 0;
    }

    uint8_t cl = inb(ata->lba_mid);
    uint8_t ch = inb(ata->lba_high);

    if (cl == 0x14 && ch == 0xEB)
        return 0; // ATAPI

    if (cl == 0x3C && ch == 0xC3)
        return 0; // SATA packet

    if (cl != 0 || ch != 0)
        return 0;
    
    // Some drives under emulation return non-zero signatures after 0xEC
    // but still provide valid ATA IDENTIFY data. We'll be more permissive.
    
    if (!wait_drq(ata)) {
        uint8_t s = inb(ata->status);
        if (s & 0x01)
            // Error bit set, might be ATAPI. Try 0xA1?
            // For now just skip.
            return 0;

        serial_print("[ATA] Identify: Timeout waiting for DRQ\n");

        return 0;
    }
    
    for (int i = 0; i < 256; i++)
        ata->metadata[i] = inw(ata->data);

    for (int i = 0; i < 20; i++) {
        uint16_t word = ata->metadata[27 + i];

        ata->model[i * 2] = (char)(word >> 8);
        ata->model[i * 2 + 1] = (char)(word & 0xFF);
    }

    ata->model[40] = '\0';

    for (int i = 39; i >= 0; i--) {
        if (ata->model[i] == ' ')
            ata->model[i] = '\0';
        else
            break;
    }

    ata->sectors = ((uint32_t) ata->metadata[61] << 16) | ata->metadata[60];
 
    char itoa_buf[32];
    itoa(ata->identifier, itoa_buf, 10);

    serial_print("ATA indntify: ATA");
    serial_print(itoa_buf);
    serial_print("\n");

    serial_print("Model: ");
    serial_print(ata->model);
    serial_print("\n");

    serial_print("Sectors: ");

    itoa(ata->sectors, itoa_buf, 10);

    serial_print(itoa_buf);
    serial_print("\n");

    return 1;
}

int read_sectors(uint32_t lba, uint32_t count, uint16_t *buffer, ata_t *ata) {
    while (count > 0) {
        uint8_t chunk = (count > 255) ? 255 : count;

        if (!ata_exists(ata)) return 0;
        if (!wait_bsy(ata)) return 0;

        outb(ata->drive_sel, (ata->slave ? 0xF0 : 0xE0) | ((lba >> 24) & 0x0F));
        ata_io_wait(ata);

        outb(ata->seccount, chunk);
        outb(ata->lba_low, (uint8_t) lba);
        outb(ata->lba_mid, (uint8_t) (lba >> 8));
        outb(ata->lba_high, (uint8_t) (lba >> 16));
        outb(ata->command, 0x20); // Read sectors
        ata_io_wait(ata);

        int sectors = (chunk == 0 ? 256 : chunk);

        for (int j = 0; j < sectors; j++) {
            if (!wait_bsy(ata)) return 0;
            if (!wait_drq(ata)) return 0;

            for (int i = 0; i < 256; i++)
                buffer[i + j * 256] = inw(ata->data);
        }

        lba += sectors;
        buffer += sectors * 256;
        count -= sectors;
    }

    return 1;
}

int write_sectors(uint32_t lba, uint32_t count, uint16_t *buffer, ata_t *ata) {
    while (count > 0) {
        uint8_t chunk = (count > 255) ? 255 : count;

        if (!ata_exists(ata)) return 0;
        if (!wait_bsy(ata)) return 0;

        outb(ata->drive_sel, (ata->slave ? 0xF0 : 0xE0) | ((lba >> 24) & 0x0F));
        ata_io_wait(ata);

        outb(ata->seccount, chunk);
        outb(ata->lba_low, (uint8_t) lba);
        outb(ata->lba_mid, (uint8_t) (lba >> 8));
        outb(ata->lba_high, (uint8_t) (lba >> 16));
        outb(ata->command, 0x30); // Write sectors
        ata_io_wait(ata);

        int sectors = (chunk == 0 ? 256 : chunk);

        for (int j = 0; j < sectors; j++) {
            if (!wait_bsy(ata)) return 0;
            if (!wait_drq(ata)) return 0;

            for (int i = 0; i < 256; i++)
                outw(ata->data, buffer[i + j * 256]);
            
            ata_io_wait(ata);

            if (!wait_bsy(ata)) return 0;
        }
        
        // Cache flush
        outb(ata->command, 0xE7);
        ata_io_wait(ata);
        wait_bsy(ata);

        lba += sectors;
        buffer += sectors * 256;
        count -= sectors;
    }
    
    return 1;
}
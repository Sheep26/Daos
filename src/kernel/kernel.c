#include <string.h>
#include <stdint.h>
#include <drivers/vga.h>
#include <drivers/system.h>
#include <drivers/io.h>
#include <multiboot.h>
#include <itoa.h>
#include <memory/pmm.h>
#include <memory/heap.h>
#include <font.h>
#include <drivers/ata.h>
#include <fs/fat32.h>
#include <fs/vfs.h>
#include <fs/nulldev.h>
#include <commands.h>
#include <thread.h>
#include <pit.h>
#include <idt.h>
#include <pic.h>
#include <timer.h>
#include <isr.h>
#include <irq.h>
#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <command_handler.h>
#include <logging.h>
#include <apic.h>
#include <drivers/system.h>

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

void* find_module(multiboot_tag_module_t* mod, const char* name, uint32_t* out_size) {
    if (strcmp(mod->cmdline, name) == 0) {
        if (out_size) *out_size = mod->mod_end - mod->mod_start;
        return (void*) mod->mod_start;
    }

    return 0;
}

void main_thread() {
    k_logln("Entering main thread");

    if (system->cpu->apic_supported) {
        // APIC is so borked.

        //k_logln("APIC available, enabling APIC.");
        //apic_enable();
    }

    clear_tty();
    reset_tty();
    log_tty = 0;

    print_tty(cwd);
    print_tty(" ");

    while (1)
        tty_input_handler(keyboard_key());
}

void kernel_main(uint32_t magic, uint32_t addr) {
    if (magic != 0x36d76289) {
        // Not booted by a Multiboot-compliant loader!
        return;
    }

    serial_init();
    k_logln("Setting up multiboot");

    uint8_t* ptr = (uint8_t*) addr;
    uint32_t total_size = *(uint32_t*) ptr;
    ptr += 8; // skip total_size + reserved

    multiboot_tag_framebuffer_t* fb_tag = 0;

    uint32_t max_addr = 0;

    multiboot_tag_mmap_t* mmap_tag;
    multiboot_mmap_entry_t* mmap_entries;
    int mmap_entry_count = 0;

    uint64_t total_usable_ram = 0;

    while (ptr < (uint8_t*)addr + total_size) {
        multiboot_tag_t* tag = (multiboot_tag_t*) ptr;

        if (tag->type == 0) break;

        if (tag->type == MULTIBOOT_MODULE_TAG) {
            // multiboot_tag_module_t* mod = (multiboot_tag_module_t*) tag;
        }

        if (tag->type == MULTIBOOT_MMAP_TAG) {
            mmap_tag = (multiboot_tag_mmap_t*) tag;

            mmap_entries = (multiboot_mmap_entry_t*) (ptr + sizeof(multiboot_tag_mmap_t));

            mmap_entry_count = (mmap_tag->size - sizeof(multiboot_tag_mmap_t)) / mmap_tag->entry_size;

            for (int i = 0; i < mmap_entry_count; i++) {
                if (mmap_entries[i].type == 1) {
                    // usable RAM
                    total_usable_ram += mmap_entries[i].len;
                    uint32_t end = mmap_entries[i].addr + mmap_entries[i].len;

                    if (end > max_addr)
                        max_addr = end;

                    // entry[i].addr -> start
                    // entry[i].len  -> size
                }
            }
        }

        if (tag->type == MULTIBOOT_FRAMEBUFFER_TAG)
            fb_tag = (multiboot_tag_framebuffer_t*) tag;

        ptr += ((tag->size + 7) & ~7); // align to 8 bytes
    }

    if (!fb_tag) {
        k_logln("ERROR: Framebuffer uninitalized");

        while (1);
    }

    k_logln("Setup multiboot");

    // Init pmm and heap.
    pmm_init(mmap_entries, max_addr, mmap_entry_count, (uint32_t) &_kernel_end);

    k_logln("Reserving framebuffer");
    pmm_reserve_region(fb_tag->framebuffer_addr, fb_tag->framebuffer_height * fb_tag->framebuffer_pitch);

    k_logln("Initalising heap.");
    heap_init(pmm_bitmap_location + pmm_bitmap_size);

    system_init();
    system->total_usable_ram = total_usable_ram;

    vga_init(fb_tag);
    tty_init(font8x8_basic);
	log_tty = 1;

    cpu_init(system->cpu);

    char buf[32];
    itoa(system->total_usable_ram / (1024 * 1024), buf, 10);
    k_log("Usable RAM: ");
    k_log(buf);
    k_logln("MB");

    k_log("CPU Vendor: ");
    k_logln(system->cpu->vendor);

    k_logln("Init VFS");
    vfs_init();

    //vfs_mount("/", ramfs_create());
    //vfs_mount("/dev/null", null_device_create());

    k_logln("Init ata0");
    ata_t *ata0 = (ata_t *) calloc(1, sizeof(ata_t));

    init_ata(ata0, ATA_PRIMARY_DATA, ATA_PRIMARY_ERR, ATA_PRIMARY_SECCOUNT, ATA_PRIMARY_LBA_LOW, ATA_PRIMARY_LBA_MID, ATA_PRIMARY_LBA_HIGH, ATA_PRIMARY_DRIVE_SEL, ATA_PRIMARY_COMMAND, ATA_PRIMARY_STATUS, 0);
    int ata0_indenify = ata_identify(ata0);

    if (!ata0_indenify) {
        k_logln("ERROR: ATA 0 not found");

        return;
    }

    k_logln("Init fat32_disk0");
    fat32_disk_t *fat32_disk0 = (fat32_disk_t *) calloc(1, sizeof(fat32_disk_t));

    if (!fat_disk_init(fat32_disk0, ata0)) {
        k_logln("Formatting disk");

        fat_format(fat32_disk0, "Disk");

        fillscreen(0x00000000);
        k_logln("Disk Formatted, please close the os.");

        return;
    }

    k_logln("Mouting fat32_disk0");
    vfs_mount("/", fat_mount_create(fat32_disk0, "FSROOT"));

    char wooo[] = "Wowwwwie we get data in the file wooooooo.";

    // mkdir_fs("/WOO", 0);
    // rm_fs("/WOO");
    // create_file_fs("/Wooo.txt", wooo, sizeof(wooo), 0);

    fs_node_t *file = kopen("/itworkie.txt", 0);

    if (!file)
        create_file_fs("/itworkie.txt", wooo, sizeof(wooo), 0);

    close_fs(file);
    free(file);

    fs_directory_t *fs_dir = (fs_directory_t *) calloc(1, sizeof(fs_directory_t));
    ls_fs("/", fs_dir);

    k_logln("FS root");

    for (int i = 0; i < fs_dir->count; i++)
        k_logln(fs_dir->nodes[i].name);
    
    free(fs_dir);

    k_logln("Init idt");
    init_idt();

    k_logln("Init isr");
    init_isr();

    k_logln("PIC remap");
    pic_remap();

    k_logln("Init irq");
    init_irq();

    k_logln("Enabling interrupts");
    enable_interrupts();

    k_logln("Setting PIC frequency");
    pit_set_frequency(PIT_FREQUENCY);

    k_logln("Setting irq handlers.");
    set_irq_handler(0, pit_timer_handler);
    set_irq_handler(1, keyboard_handler);

    set_idt_gate(APIC_TIMER_VECTOR, apic_timer_handler);

    flush_keyboard();

    create_command("help", "Open this menu", run_help);
    create_command("badapple", "Run bad apple", run_badapple);
    create_command("ls", "List files in a directory", run_ls);
    create_command("mkdir", "Make a new directory", run_mkdir);
    create_command("cat", "Read from directory", run_cat);
    create_command("cd", "Change working directory", run_cd);
    create_command("hello", "Prints hello world", run_hello);

    k_logln("Creating idle thread");
    create_idle_thread(idle_func);

    k_logln("Creating main thread");
    create_new_thread(main_thread);

    k_logln("Running schedular");
    scheduler_run();
}
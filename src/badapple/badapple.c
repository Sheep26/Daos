#include <badapple.h>

void run_badapple() {
    fillscreen(0x00000000);
    draw_string("Bad Apple Loading...", 8, 8, 0x00FFFFFFFF, 0x00000000, font8x8_basic);

    flush_buffer();

    char buf[32];
    fs_node_t *badapple_file = kopen("/fsroot/VIDEO.BIN", 0);

    if (badapple_file) {
        serial_print("Badapple size: ");

        char sizebuf[32];
        itoa(badapple_file->length, sizebuf, 10);

        serial_println(sizebuf);

        uint8_t *badapple_data = malloc(badapple_file->length);
        read_fs(badapple_file, 0, badapple_file->length, badapple_data);

        serial_println("Badapple loaded");

        /* for (uint32_t i = 0; i < badapple_file->length; i++) {
            if ((i % 16) == 0) {
                serial_print("\n");
            }

            char hex[8];

            itoa(badapple_data[i], hex, 16);

            if (badapple_data[i] < 16)
                serial_print("0");

            serial_print(hex);
            serial_print(" ");
        } */

        uint32_t badapple_width = 640;
        uint32_t badapple_height = 480;

        uint32_t bytes_per_row = (badapple_width + 7) / 8;
        uint32_t bytes_per_frame = bytes_per_row * badapple_height;

        uint32_t frames = badapple_file->length / bytes_per_frame;

        for (uint32_t frame = 0; frame < frames; frame++) {
            for (uint32_t y = 0; y < badapple_height; y++) {
                for (uint32_t byte = 0; byte < bytes_per_row; byte++) {
                    uint8_t row_byte = badapple_data[frame * bytes_per_frame + y * bytes_per_row + byte];

                    for (uint32_t bit = 0; bit < 8; bit++) {
                        uint32_t x = byte * 8 + bit;

                        uint8_t pixel = (row_byte >> (7 - bit)) & 1;

                        putpixel(x, y, pixel ? 0x00FFFFFF : 0x00000000);
                    }
                }
            }

            serial_print("Frame: ");
            itoa(frame, buf, 10);
            serial_println(buf);

            flush_buffer();
        }
    }
}
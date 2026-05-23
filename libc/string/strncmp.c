#include <string.h>

int strncmp(const char* a, const char* b, size_t n) {
    if (n == 0)
        return 0;

    while (n-- > 0) {
        unsigned char ca = (unsigned char) *a++;
        unsigned char cb = (unsigned char) *b++;

        if (ca != cb)
            return ca - cb;

        if (ca == '\0')
            return 0;
    }

    return 0;
}
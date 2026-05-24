#include <string.h>
#include <memory/liballoc/liballoc.h>

char * strdup(const char *str) {
	int len = strlen(str);
	char * out = malloc(sizeof(char) * (len+1));
	memcpy(out, str, len+1);
	return out;
}
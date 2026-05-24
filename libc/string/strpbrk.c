#include <string.h>

char *strpbrk(const char * str, const char * accept) {
	const char *acc = accept;

	if (!*str) {
		return NULL;
	}

	while (*str) {
		for (acc = accept; *acc; ++acc) {
			if (*str == *acc) {
				break;
			}
		}
		if (*acc) {
			break;
		}
		++str;
	}

	if (*acc == '\0') {
		return NULL;
	}

	return (char *)str;
}
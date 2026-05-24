#ifndef _STRING_H
#define _STRING_H

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);
int strcmp(const char* a, const char* b);
char* strcpy(char* dest, const char* src);
int strncmp(const char* a, const char* b, size_t n);
char *strdup(const char *str);
char *strtok_r(char * str, const char * delim, char ** saveptr);
size_t strspn(const char *str, const char *accept);
char *strpbrk(const char * str, const char * accept);
size_t lfind(const char * str, const char accept);

#ifdef __cplusplus
}
#endif

#endif
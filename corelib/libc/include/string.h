#ifndef _STRING_H
#define _STRING_H 1

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memccpy(void *s1, const void *s2, int c, size_t n);
void *memchr(const void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void*, const void*, size_t);
void *memset(void*, int, size_t);
char *strcpy(char*, const char*);
int strcmp(const char*, const char*);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlen(const char*);
char *strchr(const char *s, int c);
char *strstr(const char *s, const char *subs);

#ifdef __cplusplus
}
#endif

#endif

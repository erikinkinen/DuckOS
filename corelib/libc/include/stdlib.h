#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void abort(void);
int atexit(void (*)(void));
int atoi(const char*);
void free(void*);
char* getenv(const char*);
void* malloc(size_t);
void* calloc(size_t,size_t);
void *realloc(void *ptr, size_t size);
void *bsearch(const void *key, const void *base,
                     size_t nmemb, size_t size,
                     int (*compar)(const void *, const void *));
void qsort(void *base, size_t nmemb, size_t size,
                  int (*compar)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif

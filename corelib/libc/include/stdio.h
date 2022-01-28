#ifndef _STDIO_H
#define _STDIO_H
#include <stdarg.h>
#include <stddef.h>
#define SEEK_SET 0
typedef struct { int unused; } FILE;
#ifdef __cplusplus
extern "C" {
#endif
#define stdin ((FILE*)0)
#define stdout ((FILE*)1)
#define stderr ((FILE*)2)
int fclose(FILE*);
int fflush(FILE*);
FILE* fopen(const char*, const char*);
int printf(const char*, ...);
int fprintf(FILE*, const char*, ...);
size_t fread(void*, size_t, size_t, FILE*);
int fseek(FILE*, long, int);
long ftell(FILE*);
size_t fwrite(const void*, size_t, size_t, FILE*);
void setbuf(FILE*, char*);
int vfprintf(FILE*, const char*, va_list);
int puts(const char *s);
#ifdef __cplusplus
}
#endif
#endif

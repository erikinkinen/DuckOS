#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEEK_SET 0
#define EOF (-1)

typedef struct { int unused; } FILE;

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
int feof(FILE *stream);
char *fgets(char *restrict s, int n, FILE *restrict stream);
int getc(FILE *stream);
int ungetc(int c, FILE *stream);
int fscanf(FILE *restrict stream, const char *restrict format, ...);
int putc(int c, FILE *stream);
int fputs(const char *restrict s, FILE *restrict stream);

#ifdef __cplusplus
}
#endif

#endif

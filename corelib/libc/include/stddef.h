#ifndef _STDDEF_H
#define _STDDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#define NULL ((void *) 0)
#define offsetof(type, member) __builtin_offsetof(type, member)

typedef long int ptrdiff_t;
typedef short wchar_t;
typedef unsigned long int size_t;

#ifdef __cplusplus
}
#endif

#endif

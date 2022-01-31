#ifndef _FCNTL_H
#define _FCNTL_H 1

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define O_RDONLY 4

int open(const char *pathname, int flags);

#ifdef __cplusplus
}
#endif

#endif

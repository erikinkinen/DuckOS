#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIGFPE 8

int kill(pid_t pid, int sig);

#ifdef __cplusplus
}
#endif

#endif

#ifndef _SYS_STAT_H
#define _SYS_STAT_H 1

#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stat {
    uint64_t st_dev;
    uint64_t st_ino;
    mode_t st_mode;
    uint64_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    uint64_t st_rdev;
    size_t st_size;
    size_t st_blksize;
    uint64_t st_blocks;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
} stat_t;

int fstat(int fd, struct stat *buf);

#ifdef __cplusplus
}
#endif

#endif

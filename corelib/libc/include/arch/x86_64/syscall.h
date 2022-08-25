#ifndef _SYSCALL_H
#define _SYSCALL_H 1

#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint64_t sys_write(FILE *f, char  *buf, uint64_t count) {
    uint64_t ret;
	asm volatile (
			"int $0x80;\n" 
			: "=a"(ret)
			: "a"(0x1), "D"(f), "S"(buf), "d"(count)
            : "memory", "cc"
			);
	return ret;
}

static inline pid_t sys_fork() {
    uint64_t ret;
	asm volatile (
			"int $0x80;\n" 
			: "=a"(ret)
			: "a"(0x100)
            : "memory", "cc"
			);
	return ret;
}

static inline pid_t sys_getpid() {
    uint64_t ret;
	asm volatile (
			"int $0x80;\n" 
			: "=a"(ret)
			: "a"(0x101)
            : "memory", "cc"
			);
	return ret;
}

static inline void sys_waitpid(pid_t pid) {
	asm volatile (
			"int $0x80;\n" 
			:: "a"(0x102), "D"(pid)
            : "memory", "cc"
			);
}

static inline void sys_exit() {
	asm volatile (
			"int $0x80;\n" 
			:: "a"(0x103)
            : "memory", "cc"
			);
}

#ifdef __cplusplus
}
#endif

#endif

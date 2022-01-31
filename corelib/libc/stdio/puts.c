#include <stdio.h>
#include <syscall.h>

int puts(const char *s) {
	int ret = 0;
	int sret = printf(s);
	if (sret < 0) return sret;
	ret += sret;
	sret = sys_write((FILE *)1, "\n", 1);
	ret += sret;
    return ret;
}
#include "stdio.h"

int puts(const char *s) {
	int ret = 0;
	int sret = printf(s);
	if (sret < 0) return sret;
	ret += sret;
	sret = printf("\n");
	ret += sret;
    return ret;
}
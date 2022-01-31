#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <syscall.h>

static const char xdigits[16] = {
	"0123456789ABCDEF"
};

static char *fmt_x(uint64_t x, char *s, int lower) {
    if (x == 0) *--s = '0';
	for (; x; x>>=4) *--s = xdigits[(x&15)]|lower;
	return s;
}

static char *fmt_u(uint64_t x, char *s){
    if (x == 0) *--s = '0';
	for (; x; x/=10) *--s = '0' + x%10;
	return s;
}

int vfprintf(FILE *f, const char *fmt, va_list ap) {
    char buf[256];
    buf[255] = 0;
    char *buftop = buf + 255;
    int written = 0;
    char *fmtcur = (char*)fmt;
    char *wr_ptr = fmtcur;
    while (*fmtcur != 0) {
        if (*fmtcur == '%') {
            if (fmtcur > wr_ptr) {
                int wres = sys_write(f, wr_ptr, (uint64_t)fmtcur - (uint64_t)wr_ptr);
                if (wres < 0) return wres;
                else written += wres;
            }
            fmtcur++;
            switch (*fmtcur) {
                case '%': {
                    int wres = sys_write(f, fmtcur, 1);
                    if (wres < 0) return wres;
                    else written += wres;
                } break;
                case 'x': case 'X': {
                    unsigned int val = va_arg(ap, unsigned int);
                    char *str = fmt_x(val, buftop, *fmtcur & 32);
                    int wres = sys_write(f, str, strlen(str));
                    if (wres < 0) return wres;
                    else written += wres;
                } break;
                case 'd': case 'u': {
                    unsigned int val = va_arg(ap, unsigned int);
                    if (*fmtcur == 'd' && (signed)val < 0) {
                        val = (unsigned)(-(signed)val);
                        int wres = sys_write(f, "-", 1);
                    }
                    char *str = fmt_u(val, buftop);
                    int wres = sys_write(f, str, strlen(str));
                    if (wres < 0) return wres;
                    else written += wres;
                } break;
                default: return -1;
            }
            wr_ptr = fmtcur + 1;
        }
        fmtcur++;
    }

    if (fmtcur > wr_ptr) {
        int wres = sys_write(f, wr_ptr, (uint64_t)fmtcur - (uint64_t)wr_ptr);
        if (wres < 0) return wres;
        else written += wres;
    }
	return written;
}

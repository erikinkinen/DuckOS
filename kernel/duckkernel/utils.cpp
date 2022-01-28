#include <utils/utils.h>

char *itoa(unsigned long long value, char * str, unsigned long long base) {
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        *ptr++ = "0123456789ABCDEFGHIJKLMNOPQRTSUWXYZ"[value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

const void *memset(const void *ptr, unsigned char value, unsigned long long n) {
	for (unsigned long long i = 0; i < n; ++i) {
		((unsigned char *)ptr)[i] = value;
	}

	return ptr;
}
#include <string.h>

void *memccpy(void *s1, const void *s2, int c, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((char *)s1)[i] = ((const char *)s2)[i];
        if (((char *)s1)[i] == (char)c) 
            return (void *)((char *)s1 + i + 1);
    }

    return NULL;
}
#include <string.h>

size_t strlen(const char* str) {
    for (size_t res = 0;;res++) 
        if (str[res] == 0) return res;
}
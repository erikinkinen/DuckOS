#pragma once

#include <init/boot.h>
#include <memory/memory.h>
#include <utils/bitmap.h>
#include <utils/utils.h>

class PageFrameAllocator {
public:
    static void init();
    static void free(void *address, unsigned long long count);
    static void lock(void *address, unsigned long long count);
    static void *alloc();
    static void *alloc(unsigned long long count);
    static unsigned long long get_free();
    static unsigned long long get_used();

private:
    static Bitmap pageFrames;
    static unsigned long long freeMemory;
    static unsigned long long usedMemory;
};

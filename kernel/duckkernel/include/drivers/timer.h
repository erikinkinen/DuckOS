#pragma once

#if ARCH == x86_64
#include <drivers/pit.h>
#endif

class Timer {
public:
    static void init();
    static void tick();
};

#pragma once

#include <drivers/timer.h>
#include <interrupts/idt.h>

extern "C" {void tick(InterruptFrame* frame, unsigned long long rax);}

class PIT {
public:
    static void init();
};
